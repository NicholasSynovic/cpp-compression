#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#include <timer.h>

#include <sys/stat.h>
// check if file already exists (w/o using c++17)
bool file_exists( const std::string& filename )
{
    struct stat f;
    if ( stat( filename.c_str(), &f ) != -1 )
        return true;
    else
        return false;
}

#include <bzlib.h>

static int bz_compression_level = 9;
static int bz_workFactor = 0; // default
static int bz_verbose    = 0;

static int verbose = 0;

std::vector<char>
compress_chunk( const std::vector<char>& idata )
{
    using uint = unsigned int;
    const uint isize = idata.size();
          uint zsize = uint( isize * 1.02 ) + 600;

    std::vector<char> zdata( zsize );

    char* iptr = const_cast<char*>( idata.data() );
    char* zptr = const_cast<char*>( zdata.data() );

    auto ierr = BZ2_bzBuffToBuffCompress( zptr, &zsize,
                                          iptr, isize,
                                          bz_compression_level,
                                          bz_verbose,
                                          bz_workFactor );

    if ( ierr != BZ_OK ) {
        fprintf(stderr,"bzBuffToBuffCompress failed with %d\n", ierr);
        exit(1);
    }

    // Reclaim any extra memory.
    zdata.resize( zsize );

    return std::move( zdata );
}


std::array<size_t,3>
compress_stream( FILE* istream, FILE* ostream )
{
    // How big are the input chunks?
    // The compression level switch is [1-9] which means 100-900k.
    size_t chunk_size = bz_compression_level * 100 * 1024;

    int n_chunks = 0;
    size_t ibytes = 0;
    size_t obytes = 0;

    while ( not(feof(istream)) )
    {
        // Space for the input stream data.
        std::vector<char> chunk( chunk_size );

        // Read a chunk of data from the input stream.
        auto bytes_read = fread( chunk.data(), sizeof(char), chunk_size, istream );

        // for debugging

        ibytes += bytes_read;

        if (bytes_read < chunk_size)
            chunk.resize( bytes_read );

        auto odata = compress_chunk( chunk );

        obytes += odata.size();

        fwrite( odata.data(), sizeof(char), odata.size(), ostream );

        if (verbose > 1)
            fprintf(stderr,"chunk: %d, idata: %lu, odata: %lu\n", n_chunks, bytes_read, odata.size());

        n_chunks ++;
    }

    return { size_t(n_chunks), ibytes, obytes };
}


// Dummy header. The 9 may be different but the rest are part of the magic sauce.
static const char _bz2header[] = "BZh91AY&SY";

// zero-terminated string
static const unsigned char _bz2ZeroHeader[] = {'B', 'Z', 'h', '9', 0x17, 0x72, 0x45, 0x38, 0x50, 0x90, 0};

std::array<size_t,3>
decompress_stream( FILE* istream, FILE* ostream )
{
   // Create a read buffer big enough to hold the largest block possible.
   // const size_t bufSize = (9 * 110000 + 600);
   const size_t bufSize = (9 * 1024 * 100 + 600);
   std::string _bufIn(bufSize, ' ');
   char *bufIn = const_cast<char*>( _bufIn.data() );

   // The data stream that'll actually be processed.
   std::string streamIn;

   std::string bz2header = _bz2header;
   // std::cerr << bz2header << " length = " << bz2header.length() << " length = " << sizeof(_bz2header) << std::endl;

   size_t ibytes = 0, obytes = 0;
   int nblocks = 0;

   int blockSize100k = 0;

   bool isFirstBlock = true;

   while (true)
   {
      auto t0 = getTimeStamp();

      // Read in a chunk of data for scanning.
      size_t nread = fread(bufIn, sizeof(char), bufSize, istream);

      // Test if we've hit the EOF or some other error.
      bool isEnd = feof(istream);
      if (nread < bufSize)
         if (not(isEnd))
         {
            fprintf(stderr,"Error reading file %d\n", __LINE__);
            exit(1);
         }

      // Concatenate the existing stream (anything left over) with the new chunk.
      streamIn.append(bufIn, bufIn + nread);
#ifdef _DEBUG
      fprintf(stderr,"nread = %lu, isEnd=%d length=%lu\n", nread, isEnd, streamIn.length());
#endif

      // Special code if this is the precise head of a block.
      if (isFirstBlock)
      {
         isFirstBlock = false;

         if (nread < bz2header.length())
         {
            fprintf(stderr,"File not a bz2 file\n");
            exit(1);
         }

         size_t pos = streamIn.find( bz2header.c_str(), 0, 3);
         if (pos != 0)
         {
            fprintf(stderr,"File not a bz2 file 2\n");
            exit(1);
         }

         // Deduce the block size used and assume all blocks are the same.
         blockSize100k = atoi(streamIn.substr(3,1).c_str());
         bz2header[3] = streamIn[3];
#ifdef _DEBUG
         std::cerr << "blockSize = " << blockSize100k << " bz2header " << bz2header << "\n";
#endif
      }

      size_t pos;

      // Sanity check that the stream starts with a full header.
      {
         pos = streamIn.find( bz2header, 0 );
         if (pos != 0)
         {
            fprintf(stderr,"bz2header not valid %d %s\n", __LINE__, streamIn.substr(0, bz2header.length()).c_str());
            exit(1);
         }
      }

      int numBlocksRead = 0;
      std::vector< std::vector<char> > idata;
      std::vector< uint32_t > isize;

      // Scan the loaded data stream and split into bz blocks. There will likely be a bit left over.
      size_t next;
      while ((next = streamIn.find( bz2header, pos + bz2header.length())) != std::string::npos or isEnd)
      {
         if (next == std::string::npos and isEnd)
            next = streamIn.length();

         int i = numBlocksRead++;
         size_t size = next - pos;
         auto begin = streamIn.data() + pos;
         std::vector<char> block( streamIn.data() + pos, streamIn.data() + next );
         isize.push_back( size );
         idata.push_back( block );

#ifdef _DEBUG
         fprintf(stderr,"found bzheader at %d %lu %lu %s %d last=%d\n", i, pos, size, streamIn.substr(next, bz2header.length()).c_str(), numBlocksRead, not(next < nread));
#endif

         pos = next;
         if (next == streamIn.length() and isEnd)
            break;
      }

      // Append the remaining segment into the stream buffer.
      streamIn.erase(0, pos);

      ibytes += nread;

      auto t1 = getTimeStamp();

      std::vector< std::vector<char> > odata( numBlocksRead );
      std::vector< uint32_t > osize( numBlocksRead, 0 );
      std::vector< int      > err( numBlocksRead, 0 );

      // Decompress the loaded blocks.
      for (int i = 0; i < numBlocksRead; i++)
      {
         osize[i] = blockSize100k * 102400;
         odata[i].resize( osize[i] );

         auto _t0 = getTimeStamp();
         int ierr = BZ2_bzBuffToBuffDecompress
                              ( odata[i].data(), &osize[i],
                                idata[i].data(),  isize[i],
                                0,
                                bz_verbose);

         err[i] = ierr;
         auto _t1 = getTimeStamp();

#ifdef _DEBUG
         fprintf(stderr,"BZ2_bzBuffToBuffDecompress: %d %d %u %u %f\n", ierr, i, osize[i], isize[i], getElapsedTime( _t0, _t1));
#endif
      }

      // Write out the decompressed streams.
      for (int i = 0; i < numBlocksRead; i++)
      {
         if ( isize[i] > 0 )
            if ( err[i] == BZ_OK )
            {
               fwrite( odata[i].data(), sizeof(char), osize[i], ostream );
               obytes += osize[i];

               osize[i] = 0;
            }
            else
            {
               fprintf(stderr,"Error decompressing block %d %d\n", i, err[i]);
               exit(1);
            }
      }

      nblocks += numBlocksRead;

      if (isEnd)
         break;
   }

#ifdef _DEBUG
   fprintf(stderr, "Decompress: BytesIn=%lu, BytesOut=%lu, %d\n", ibytes, obytes, nblocks);
#endif

   return { size_t(nblocks), ibytes, obytes };
}

void usage(FILE *os)
{
    fprintf(os, "Usage: pbzip2 <options> <files>\n");
    fprintf(os, " -h | --help           print this message\n");
    fprintf(os, " -l | --level=<#>      compression level [1-9] (default: 9)\n");
    fprintf(os, " -c | --stdout         write the compressed data to stdout instead of <input>.bz2\n");
    fprintf(os, " -i | --stdin          read the input data from stdin instead of a file.\n");
    fprintf(os, " -v | --verbose        increment the verbosity level.\n");
    fprintf(os, " -f | --force          force overwrite of exiting .bz2 file if present.\n");
    fprintf(os, " -d | --decompress     decompress the file.\n");
    fprintf(os, " -z | --compress       compress the file (default).\n");
}

std::string to_string( const bool value ) { return (value) ? "true" : "false"; }

template <typename T>
std::string to_string (const std::vector<T>& v)
{
    std::stringstream ss;

    ss << "[";
    for (int i = 0; i < v.size(); ++i)
    {
        ss << v[i];
        if (i == v.size()-1)
            ss << "]";
        else
            ss << ", ";
    }

    return ss.str();
}


int main (int argc, char* argv[])
{
    bool from_stdin = false;
    bool to_stdout = false;
    bool force_overwrite = false;
    bool compress = true;
    std::vector< std::string > in_files;

    for (int i = 1; i < argc; )
    {
        std::string key = argv[i++];

        if ( key == "-h" or key == "--help")
        {
            usage(stdout);
            return 0;
        }
        else if (key == "-l" or key == "--level")
            bz_compression_level = atoi( argv[i++] );
        else if (key == "-d" or key == "--decompress")
            compress = false;
        else if (key == "-z" or key == "--compress")
            compress = true;
        else if (key == "-v" or key == "--verbose")
            verbose++;
        else if (key == "-c" or key == "--stdout")
            to_stdout = true;
        else if (key == "-i" or key == "--stdin")
            from_stdin = true;
        else if (key == "-f" or key == "--force")
            force_overwrite = true;
        else {
            in_files.push_back( key );
        }
    }

    if (from_stdin)
    {
        if (not(to_stdout) and verbose)
            fprintf(stderr,"Warning: writing to stdout by default when reading from stdin.\n");

        to_stdout = true;

        if ( in_files.size() != 0 ) {
            fprintf(stderr, "Error: cannot read from stdin and a file list\n");
            return 1;
        }
    }
    else {
        if ( in_files.size() == 0 ) {
            fprintf(stderr, "Error: no input files given\n");
            return 1;
        }
    }

    if (verbose > 1) {
        std::cerr << "Options:"
                  << " compression_level= " << bz_compression_level
                  << " verbose=" << to_string(verbose )
                  << " force="   << to_string(force_overwrite )
                  << " stdout="  << to_string(to_stdout )
                  << " stdin="   << to_string(from_stdin )
                  << " compress="   << to_string(compress)
                  << " files="   << to_string( in_files ) << "\n";
    }

    if (from_stdin)
        in_files.push_back( "(stdin)" );

    size_t max_filename_length = 0;
    for (int i = 0; i < in_files.size(); ++i)
        max_filename_length = std::max( max_filename_length, in_files[i].length() );


    for (auto ifile: in_files)
    {
        FILE *istream = stdin;
        if ( not(from_stdin) )
        {
            if ( not(compress) ) {
                auto len = ifile.length();
                if ( len < 4 ) {
                    fprintf(stderr,"Input file name too short %s\n", ifile.c_str());
                    return 1;
                }
                std::string ext( ifile, ifile.length() - 4, 4 );
                if ( ext != ".bz2" ) {
                    fprintf(stderr,"Input file name does not have bz2 extension %s\n", ifile.c_str());
                    return 1;
                }
            }

            istream = fopen(ifile.c_str(), "rb");
            if ( istream == nullptr ) {
                fprintf(stderr,"Error opening input file %s\n", ifile.c_str());
                return 1;
            }
        }

        FILE *ostream = stdout;
        if ( not(to_stdout) )
        {
            std::string ofile = ifile + ".bz2";
            if ( not(compress) ) ofile = std::string( ifile, 0, ifile.length() - 4);
            std::cerr << ofile << '\n';

            if ( not(force_overwrite) and file_exists(ofile) ) {
                fprintf(stderr, "Error: will not overwrite exiting .bz2 file %s. To overwrite, specify --force", ofile.c_str());
                return 2;
            }

            ostream = fopen( ofile.c_str(), "wb" );
            if ( ostream == nullptr ) {
                fprintf(stderr, "Error: failed to open %d for writing\n", ofile.c_str());
                return 3;
            }
        }

        if (verbose) {
            std::stringstream ss;
            ss << "  %" << max_filename_length <<  "s: ";
            fprintf(stderr, ss.str().c_str(), ifile.c_str() );
        }

        std::array<size_t,3> res;
        if (compress)
            res = compress_stream( istream, ostream );
        else
            res = decompress_stream( istream, ostream );

        if ( not(from_stdin) )
            fclose( istream );

        if ( not(to_stdout) )
            fclose( ostream );

        if (verbose)
            if ( compress )
            {
                size_t ibytes = res[1];
                size_t obytes = res[2];

                double ratio = double(ibytes) / double(obytes);
                double bits = 8. * double(obytes) / double(ibytes);
                double percent = 100. * (double(ibytes) - double(obytes)) / double(ibytes);

                fprintf(stderr, "%6.3f:1, %6.3f bits/byte, %5.2f%% saved, %lu in, %lu out.\n", ratio, bits, percent, ibytes, obytes);
                //"""   openmpi/openmpi-4.1.4.tar: 11.454:1,  0.698 bits/byte, 91.27% saved, 115025920 in, 10042839 out."""
            }
            else
                fprintf(stderr, "done\n");
    }

    return 0;
}
