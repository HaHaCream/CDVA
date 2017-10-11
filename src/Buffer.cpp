/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "Buffer.h"
#include <cstring>   // std::memcpy, std::memcmp, std::memset, std::memchr
#include <string>
#include <fstream>
#include <algorithm>
#include "CdvaException.h"

using namespace mpeg7cdva;

#define MyAbs(a) (a>=0)?(a):(-a)

Buffer::Buffer():mydata(NULL),mysize(0)
{
}

Buffer::Buffer(unsigned char * newdata, size_t newsize):mydata(NULL),mysize(0)
{
	assign(newdata, newsize);
}

Buffer::~Buffer()
{
	if (mydata != NULL)
    delete[] mydata;
}


Buffer::Buffer (size_t size):mydata(NULL),mysize(0)
{
	if (size > 0)
	{
		mydata = new unsigned char [size];
		mysize = size;		
	}
}

Buffer::Buffer (const Buffer& buffer):mydata(NULL),mysize(0)
{
	if (buffer.size() > 0)
	{
		mydata = new unsigned char [buffer.size()];
		mysize = buffer.size();
		memcpy (mydata, buffer.mydata, buffer.mysize);
	}
}

Buffer& Buffer::operator= (const Buffer& x)
{	
	if (&x != this)
	{
		if (x.mysize == 0)
		{
			if (mydata != NULL)
				delete[] mydata;
			mydata = NULL;
			mysize = 0;
			return *this;
		}
		
		if (x.mysize > mysize)
		{
		  if (mydata != NULL)
			delete[] mydata;

		  mydata = new unsigned char[x.mysize];
		  mysize = x.mysize;
		}

		if (x.mysize > 0)
		  memcpy (mydata, x.mydata, x.mysize);

		mysize = x.mysize;
	}

	return *this;	
}

void Buffer::swap (Buffer& x)
{
  unsigned char* d  = x.mydata;
  size_t s  = x.mysize;

  x.mydata = mydata;
  x.mysize = mysize;
 
  mydata = d;
  mysize = s;
}


void Buffer::fill (unsigned char value)
{
	if (mysize > 0)
		memset (mydata, value, mysize);
}

size_t Buffer::size () const
{
	return mysize;
}

bool Buffer::resize (size_t newsize)
{
  if (newsize <= 0)
	  return false;
	  
  if (newsize <= mysize)	// size decrease request
  {
	  mysize = newsize;
	  return true;
  }
  
  unsigned char* d = new unsigned char[newsize];

  if (mydata != NULL)
    delete[] mydata;

  mydata = d;
  mysize = newsize;
 
  return true;
}
 
bool Buffer::empty () const
{
	return (mysize == 0);
}

void Buffer::clear ()
{
	if (mydata != NULL)
	{
		delete[] mydata;
		mydata = NULL;
	}
	mysize = 0;
}

bool Buffer::assign(const unsigned char * newdata, size_t newsize)
{
	if ((newsize <= 0) || (newdata == NULL))
		return false;
	
	if (newsize > mysize)
	{
	  if (mydata != NULL)
		delete[] mydata;

	  mydata = new unsigned char[newsize];
	}

	memcpy (mydata, newdata, newsize);
	mysize = newsize;
	return true;
}

bool Buffer::equals(Buffer & buffer)
{
	if (buffer.mysize != mysize)
		return false;
	
	return (memcmp (data(), buffer.data(), buffer.size()) == 0);		
}

unsigned char* Buffer::data ()
{
	return mydata;
}

const unsigned char* Buffer::data () const
{
	return mydata;
}

char* Buffer::sdata ()
{
	return (char *) mydata;
}

const char* Buffer::sdata () const
{
	return (const char *) mydata;
}

/*
 * Read a file content into the buffer.
 */
void Buffer::read(const char * fname)
{
	std::ifstream infile(fname, std::ifstream::binary);
	if (infile.fail())
		throw CdvaException(std::string("Cannot open file ").append(fname));

	// get length of file:
	infile.seekg (0, infile.end);
	std::streamoff length = infile.tellg();
	infile.seekg (0, infile.beg);

	if (length > 0)
	{
		resize(length);		// allocate memory if needed
		infile.read (sdata(), length);	// read data as a block
		if (infile.fail())
			throw CdvaException(std::string("Error reading ").append(fname));
	}
	else
		clear();

	infile.close();
}


void Buffer::write(const char * fname) const
{
	std::ofstream outfile(fname, std::ofstream::binary);
	if (outfile.fail())
		throw CdvaException(std::string("Cannot open file ").append(fname));

	outfile.write (sdata(), size());	// write data as a block
	if (outfile.fail())
		throw CdvaException(std::string("Error writing ").append(fname));

	outfile.close();
}

int Buffer::compare(const Buffer & other) const
{
	size_t minsize = std::min(mysize, other.mysize);
	const unsigned char * a = data();
	const unsigned char * b = other.data();
	int count = 0;

	for (size_t k=0; k<minsize; ++k)
	{
		if (*(a++) != *(b++))
				count++;
	}

	if (mysize != other.mysize)
		count += MyAbs((int)(mysize - other.mysize));

	return count;
}

bool Buffer::operator== (const Buffer& other) const
{
	return (compare(other) == 0);
}
