// Copyright 2023 Zuse Institute Berlin
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


 #include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <array>
#include <chrono>
#include <numeric>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <bits/stdc++.h> 

namespace timer = std::chrono;
namespace fs = std::filesystem;

class sync_filebuf : public std::filebuf {
public:
    sync_filebuf (std::ofstream &fout) {
        sync_filebuf *fbuf;
        fbuf = static_cast<sync_filebuf *> (fout.rdbuf());
        fsync(fbuf->_M_file.fd());
    }
};

class image {
    public:
        std::string magic_identifier;
        long n, m, maxval;
        std::streampos image_offset;
        std::ifstream *ifdp;
        std::ofstream *ofdp;
        char *bytes;
    
        image (std::ifstream &ifd) : ifdp (&ifd) {
            // read the header
            (*ifdp) >> magic_identifier >> n >> m >> maxval;
            image_offset = (*ifdp).tellg ();
            bytes = NULL;
        }

        image (std::ofstream &ofd) : ofdp (&ofd) {
            bytes = NULL;
        }

        void fill_header (long _n, long _m, long _maxval, std::string _magic_identifier) {
            if (not ofdp) {
                std::cout << "read only fstream" << std::endl;
                return;
            }

            n = _n;
            m = _m;
            maxval = _maxval;
            magic_identifier = _magic_identifier;
            
            (*ofdp) << magic_identifier << "\n" <<  n << "\t" << m << "\n" << maxval << std::endl;
    
            image_offset = (*ofdp).tellp (); 
        }
        
        char get_image_pixel (int x, int y) {
            if (bytes != NULL) {
                return bytes [y*n + x];
            }
            else {
                long seek_size = y*n + x;
                ifd ().seekg (image_offset + seek_size);
                return ifd ().get ();
            }
        }

        void load_image (long chunk) {
            if (ifdp and bytes == NULL) {
                //ifd ().seekg (0, std::ifstream::end);
                //auto datasize = ifd ().tellg () - image_offset;
                //ifd ().seekg (image_offset);
                long datasize = n*m;
                bytes = (char *) malloc (datasize * sizeof (char));
                long readb = 0, chunkb;
                ifd ().seekg (image_offset);
                while (readb < datasize) {
                    chunkb = (chunk > (datasize - readb)) ? (datasize - readb) : chunk;
                    ifd ().read (bytes + readb, chunkb);
                    readb += chunkb;
                }
            }
        }

        std::ifstream &ifd () {
            return *ifdp;
        }

        std::ofstream &ofd () {
            return *ofdp;
        }
};


void sparse_collect_pixels (image source, image dest, int step) {

    int n = source.n;
    int m = source.m;
    char p;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header ((int) (n/step), 
            (int) (m/step), 
            source.maxval, 
            source.magic_identifier);

    auto image_offset = dest.image_offset;

    for (int j = 0; j < m; j += step) {
        for (int i = 0; i < n; i += step) {
            p = source.get_image_pixel (i, j);
            ofd.put (p);
            ofd.flush ();
            sync_filebuf syncbuf (ofd);
        }
    }
}

void rotate_image (image source, image dest) {
    
    int n = source.n;
    int m = source.m;
    char p;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header (m, n, 
            source.maxval, 
            source.magic_identifier);

    auto image_offset = dest.image_offset;

    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i ++) {
            p = source.get_image_pixel (j, i);
            ofd.put (p);
            ofd.flush ();
            sync_filebuf syncbuf (ofd);
        }
    }
}

void compress_image (image source, image dest, int step) {

    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header ((int) (n/step), 
            (int) (m/step), 
            source.maxval, 
            source.magic_identifier);
   
    auto image_offset = dest.image_offset;
    
    std::vector <char> buffer (step);
    uint16_t val;

    for (int j = 0; j < m; j+=step) {
        val = 0;
        for (int i = 0; i < n; i+=step) {
            for (int k = 0; k < step; k++) {
                if ((j+k) > m) continue;
                source.ifd ().seekg ((j+k)*n + i);
                source.ifd ().read (buffer.data (), step);
            }
            val += std::accumulate (buffer.begin (), buffer.end (), 0);
        }
        val = val/(step*step);
        ofd.put (val);
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
    }

}

void posterize_image (image source, image dest, long chunk, int colors) {
    
    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header (n, 
            m, 
            source.maxval, 
            source.magic_identifier);
   
    auto image_offset = dest.image_offset;
    ofd.seekp (image_offset);

    long writtenb = 0, chunkb;
    long datasize = n*m;

    int colorblocks = (int)(source.maxval/(colors-1));

    for (int i = 0; i < n*m; i ++) {
        int no_of_blocks = ((int)(source.bytes[i]/(colorblocks)));
//        std::cout << no_of_blocks << " " << (uint8_t)source.bytes[i] << " " << colorblocks << std::endl;
        if (source.bytes[i] - no_of_blocks*colorblocks > (no_of_blocks+1)*colorblocks - source.bytes[i]) {
            source.bytes[i] = (no_of_blocks+1)*colorblocks;
        }
        else {
            source.bytes[i] = no_of_blocks*colorblocks;
        }
//        std::cout << no_of_blocks << " after " << (uint8_t)source.bytes[i] << " " << colorblocks << std::endl;
    }

    while (writtenb < datasize) {
        chunkb = (chunk > (datasize - writtenb)) ? (datasize - writtenb) : chunk;
        ofd.write (source.bytes+writtenb, chunkb);
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
        writtenb += chunkb;
    }
   
}

// sequential chunks
void multiply_image (image source, image dest, int nx, int mx, long chunk) {
    
    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header ((int) (n*nx), 
            (int) (m*mx), 
            source.maxval, 
            source.magic_identifier);
   
    auto image_offset = dest.image_offset;


    for (int i = 0; i<nx*mx; i++) {
        long writtenb = 0, chunkb;
        long datasize = n*m;
        while (writtenb < datasize) {
            chunkb = (chunk > (datasize - writtenb)) ? (datasize - writtenb) : chunk;
            ofd.write (source.bytes+writtenb, chunkb);
            ofd.flush ();
            sync_filebuf syncbuf (ofd);
            writtenb += chunkb;
        }
    }
    
}

// stripe chunks
void stripe_image (image source, image dest, long chunk) {
    
    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header (n, m, 
            source.maxval, 
            source.magic_identifier);
   
    auto image_offset = dest.image_offset;
    long writtenb = 0;
    long datasize = n*m;
    long nchunks = (int)(datasize/chunk);
    int half = (int)(m*n/2);
    long offset1 = image_offset, offset2 = half + image_offset;

    for (int i = 0; i < nchunks; i ++) {
        if (i%2) {

            ofd.seekp (offset1);
            offset1 += chunk;
        }
        else {
            ofd.seekp (offset2);
            offset2 += chunk;
        }
        ofd.write (source.bytes+i*chunk, chunk);
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
        writtenb += chunk;
    }

    if (writtenb < datasize) {
        if (datasize - writtenb >= chunk) {
            std::cout << writtenb << " >= " << chunk << std::endl;
        }
        ofd.seekp (writtenb + image_offset);
        ofd.write (source.bytes+writtenb, (datasize-writtenb));
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
    }

    
}

// random chunks
void rearrange_image (image source, image dest, long chunk) {
    
    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header (n, m, 
            source.maxval, 
            source.magic_identifier);
   
    auto image_offset = dest.image_offset;
    long writtenb = 0;
    long datasize = n*m;
    long nchunks = (int)(datasize/chunk);
    long offset;
    int step = 385;

    for (int s = 0; s < step; s++) {
        for (long i=s; i < nchunks; i += step) {
            offset = i*chunk + image_offset;
            ofd.seekp (offset);
            long res = ofd.tellp (); 
            if (res != offset)
                std::cout << res << " != " << offset << std::endl;
            ofd.write (source.bytes+i*chunk, chunk);
            ofd.flush ();
            sync_filebuf syncbuf (ofd);
            writtenb += chunk;
        }
    }
    if (writtenb < datasize) {
        if (datasize - writtenb >= chunk) {
            std::cout << writtenb << " >= " << chunk << std::endl;
        }
        ofd.seekp (writtenb + image_offset);
        ofd.write (source.bytes+writtenb, (datasize-writtenb));
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
    }

    
}


// random chunks
void shuffle_image (image source, image dest, long chunk) {
    
    int n = source.n;
    int m = source.m;

    std::ofstream &ofd = dest.ofd ();

    dest.fill_header (n, m, 
            source.maxval, 
            source.magic_identifier);
   
    unsigned seed = 0;
          

    auto image_offset = dest.image_offset;
    long writtenb = 0;
    long datasize = n*m;
    long nchunks = (int)(datasize/chunk);
    long offset;

    long chunkorders[nchunks];
    for (int i = 0; i<nchunks; i++) {
        chunkorders[i] = i;
    }

    std::shuffle(chunkorders, chunkorders + nchunks, std::default_random_engine(seed));

    for (long i=0; i < nchunks; i ++) {
        offset = chunkorders[i]*chunk + image_offset;
        ofd.seekp (offset);
        long res = ofd.tellp (); 
        if (res != offset)
            std::cout << res << " != " << offset << std::endl;
        ofd.write (source.bytes+i*chunk, chunk);
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
        writtenb += chunk;
    }

    if (writtenb < datasize) {
        if (datasize - writtenb >= chunk) {
            std::cout << writtenb << " >= " << chunk << std::endl;
        }
        ofd.seekp (writtenb + image_offset);
        ofd.write (source.bytes+writtenb, (datasize-writtenb));
        ofd.flush ();
        sync_filebuf syncbuf (ofd);
    }

    
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Wrong usage, blocksize is expected" << std::endl;
        exit(1);
    } 
    long chunk = std::stol (argv[1], nullptr, 10);
    std::cout << "Blocksize " << chunk << std::endl;

    auto start = timer::high_resolution_clock::now ();

    std::string source ("image_1GB");
    std::string dest ("/local2/bzcghola/image_out");

// TODO: to redeirect IO, first check if the current directory is not the optimal one

    std::ifstream ifd;    
//    #pragma io sig totalsize(1-GB) size_per_io(64-MB) 
    ifd.open (source, std::ifstream::in | std::ifstream::binary);
    image image_in (ifd);
    
    std::cout << "Reading PGM " << image_in.magic_identifier 
        << " of size " << image_in.n << " x " 
        << image_in.m << std::endl;

    long source_size = fs::file_size (source);
    image_in.load_image (chunk);

    auto ifstop = timer::high_resolution_clock::now ();

    std::ofstream ofd;
//    #pragma io sig totalsize(1-GB) size_per_io(512-KB) random_access
    ofd.open (dest, std::ofstream::out);
    image image_out (ofd);
    fs::resize_file (dest, source_size);

    auto ofstop = timer::high_resolution_clock::now ();

//    rearrange_image (image_in, image_out, chunk);
//    multiply_image (image_in, image_out, 1, 1, chunk);
//    shuffle_image (image_in, image_out, chunk);
//    rotate_image (image_in, image_out);
//    stripe_image (image_in, image_out, chunk);
    posterize_image (image_in, image_out, chunk, 10);

    auto xstop = timer::high_resolution_clock::now ();


    ofd.close ();
    ifd.close (); 

    auto stop = timer::high_resolution_clock::now ();
    auto totalduration = timer::duration_cast<timer::milliseconds> (stop - start); 
    auto ifduration = timer::duration_cast <timer::milliseconds> (ifstop - start);
    auto ofduration = timer::duration_cast <timer::milliseconds> (ofstop - ifstop);
    auto xduration = timer::duration_cast <timer::milliseconds> (xstop - ofstop);
    std::cout << "input image open took " << ifduration.count () << " ms" << std::endl;
    std::cout << "output image open took " << ofduration.count () << " ms" << std::endl;
    std::cout << "IO operations took " << xduration.count () << " ms" << std::endl;
    std::cout << "total duration " << totalduration.count () << " ms" << std::endl;
	return 0;


}

