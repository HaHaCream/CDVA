/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <cstddef>   // std::size_t

namespace mpeg7cdva
{


/**
 * @class Buffer
 * A container class for a byte array, intended to replace all malloc() and new() instructions in the main code.
 * This class properly deallocates memory when an exception is thrown.
 * @author Massimo Balestri
 * @date 2013
 */
class Buffer
{
public:
  Buffer();
  virtual ~Buffer();
  Buffer (size_t size);								///< create a buffer of the given size
  Buffer (unsigned char * data, size_t size);		///< copy the given array into this Buffer

  Buffer (const Buffer&);					///< copy the given Buffer into this Buffer
  Buffer& operator= (const Buffer&);		///< assign a Buffer to another

  void swap (Buffer& x);					///< swap the content of two Buffer(s)
  void fill (unsigned char value = 0);		///< fill a Buffer with the given value

  size_t size () const;						///< return the current size of the Buffer
  
  bool resize (size_t newsize);				///< change buffer size; content is lost if newsize if less than the current size
 
  bool empty () const;				///< return true if the Buffer is empty

  void clear ();					///< clear the Buffer

  bool assign(const unsigned char * data, size_t size);		///< assign the given data to Buffer

  bool equals(Buffer & buffer);			///< compare if two Buffer(s) are equal (i.e. if they have the same size and contain the same data)

  unsigned char* data ();				///< access to Buffer's data as unsigned char (writable)

  const unsigned char* data () const;	///< access to Buffer's data as unsigned char (read only)

  char* sdata ();				///< access to Buffer's data as signed char (writable)

  const char* sdata () const;	///< access to Buffer's data as signed char (read only)

  void read(const char * fname);		///< read Buffer from a file

  void write(const char * fname) const;	///< write Buffer to file

  /**
   * Compare this buffer with another; return the number of different bytes.
   * @param other the other Buffer
   * @return the number of differences; zero if no difference is found.
   */
  int compare(const Buffer & other) const;

  bool operator== (const Buffer& other) const;		///< compare if two Buffer(s) are equal (i.e. if they have the same size and contain the same data) 

private:
  unsigned char* mydata;
  size_t mysize;
};


} // end of namespace
