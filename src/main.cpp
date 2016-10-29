#include "ptr_resource.hpp"
#include "utils.hpp"
#include "zip.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std;

int main( int argc, char* argv[] )
{
    try {

    if( argc != 3 ) {
        cerr << "Usage: p-unzip N file.zip" << endl;
        cerr << "  where `N` is number of threads." << endl;
        return 1;
    }

    string filename = argv[2];
    log( "File: " + filename );

    int jobs = atoi( argv[1] );
    if( jobs <= 0 )
        throw runtime_error( "invalid number of jobs" );
    log( "Jobs: " + string( argv[1] ) );

    File f( filename, "rb" );
    log( "Opened file " + filename );

    if( fseek( f.get(), 0, SEEK_END ) != 0 )
        throw runtime_error( "failed to seek to end of file" );
    size_t length = ftell( f.get() );
    cout << "Length: " << length << endl;
    rewind( f.get() );

    vector<uint8_t> buffer( length );
    if( buffer.size() != length )
        throw runtime_error( "invalid buffer size" );

    void* start = &buffer[0];
    auto length_read = fread( start, 1, length, f.get() );
    if( length != length_read )
        throw runtime_error( "failed to read zip file" );
    cout << "Read " << length_read << " bytes" << endl;

    /////////////////////////////////////////////////////////////

    ZipSource zs( start, length );

    Zip z( zs );

    auto count = zip_get_num_entries( z.get(), ZIP_FL_UNCHANGED );
    cout << "Found " << count << " entries:" << endl;

    for( auto i = 0; i < count; ++i ) {

        zip_stat_t s;
        if( zip_stat_index( z.get(), i, ZIP_FL_UNCHANGED, &s ) )
            throw runtime_error( "failed to stat file" );

        if( s.valid & ZIP_STAT_INDEX     ) cout << "  index: "       << s.index     << endl;
        if( s.valid & ZIP_STAT_NAME      ) cout << "    name:      " << s.name      << endl;
        if( s.valid & ZIP_STAT_SIZE      ) cout << "    size:      " << s.size      << endl;
        if( s.valid & ZIP_STAT_COMP_SIZE ) cout << "    comp_size: " << s.comp_size << endl;
        if( s.valid & ZIP_STAT_MTIME     ) cout << "    mtime:     " << s.mtime     << endl;
        if( s.valid & ZIP_STAT_FLAGS     ) cout << "    flags:     " << s.flags     << endl;
    }

    /////////////////////////////////////////////////////////////

    } catch( exception const& e ) {
        cerr << "exception: " << e.what() << endl;
        return 1;
    }
}
