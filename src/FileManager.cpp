/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "FileManager.h"
#include "CdvaException.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;
using namespace mpeg7cdva;

FileManager::FileManager(const char *filepathname, int level):lines(),irlevel(level),hasWorkspaceDir(false)
{
	readAnnotation(filepathname);
}

FileManager::~FileManager() {
}

string FileManager::getDatasetPathName() const
{
	return datasetBaseDir + "/" + datasetName;
}

string FileManager::getDatasetPath() const
{
	return datasetBaseDir;
}

string FileManager::getDatasetName() const
{
	return datasetName;
}

size_t FileManager::getDatasetSize() const
{
	return ds_size;
}

void FileManager::setWorkspaceDir(const char * workdir)
{
	if (workdir != NULL)
	{
		workspaceDir = workdir;
		hasWorkspaceDir = true;
	}
}

std::string FileManager::getWorkspaceDir() const
{
	return workspaceDir;
}

void FileManager::appendAnnotation(string line)
{
	if (irlevel < 5)	// no more than 5 level of recursion supported (to avoid infinite loops)
	{
		do
		   line.erase(0,1);			// erase 1 character
		while(line[0] == ' ');

		// if the pathname start with "/" it's an absolute path; as such, must be rejected (only relative paths are portable).
		if ((line[0] == '/') || (line[0] == '\\'))
		   throw CdvaException(string("Absolute path found in annotation: ").append(line));

		string includefilename = datasetBaseDir + "/" + line;
		FileManager other(includefilename.c_str(), irlevel + 1);

		std::size_t found = line.find_last_of("/\\");
		if (found == string::npos) 	// same level of directory: just copy the file names
		{
		   lines.insert(lines.end(), other.lines.begin(), other.lines.end());
		}
		else		// subdir: must add subdir name to each pathname (query and reference - but the reference may be missing)
		{
		   string path = line.substr(0,found);
		   int num = other.countNames(0);
		   if (num == 1)			// has only queries
		   {
			   for (size_t i=0; i<other.lines.size(); ++i)
			   {
				   lines.push_back(path + "/" + other.getQueryName(i, false));
			   }
		   }
		   else if (num >= 2)		// has queries and references
		   {
			   for (size_t i=0; i<other.lines.size(); ++i)
			   {
				   lines.push_back(path + "/" + other.getQueryName(i, false) + " " + path + "/" + other.getReferenceName(i, false));
			   }
		   }

		}
	}
}

size_t FileManager::readAnnotation(const char *filepathname)
{
	string str(filepathname);						// pathname of text file containing the list of videos
	std::size_t found = str.find_last_of("/\\");
	datasetBaseDir = (found ==  string::npos)? "." : str.substr(0,found).c_str();
	datasetName = str.substr(found+1).c_str();

   ifstream file(filepathname);
   if (! file.good())
   {
	   throw CdvaException(string("Cannot open file: ").append(filepathname));
   }

   lines.clear();
   string line;
   while( getline( file, line ))
   {
	   if (line.size() > 1)
	   {
		   if (line[0] == '@')				// include other annotation file
		   {
			   appendAnnotation(line);
		   }
		   else if (line[0] != '#')				// skip comments
		   {
			   lines.push_back( line );
		   }
	   }
   }

   ds_size = lines.size();
   return ds_size;
}

string FileManager::replaceExt(const string & imageName, const char * ext) const
{
	string newname;
	if (hasWorkspaceDir)	// in this case, use workspaceDir in place of datasetBaseDir
		newname = workspaceDir + imageName.substr(datasetBaseDir.length());
	else
		newname = imageName;

	size_t found = newname.find_last_of('.');
	return newname.substr(0,found) + ext;
}

string FileManager::replacePath(const std::string & imageName, const char * newpath)
{
	std::size_t found = imageName.find_last_of("/\\");
	return string(newpath) + "/" + imageName.substr(found+1);
}

string FileManager::getQueryName(size_t i, bool absolutePathname) const
{
	if (i >= lines.size())
		throw CdvaException("FileManager::getQueryName index out of range");

	istringstream iss(lines[i]);
	string sub;
	iss >> sub;										// the query is the first name

	if (iss.fail())
		return string("");

	if (absolutePathname)
		return (datasetBaseDir + "/" + sub);		// absolute pathname
	else
		return (sub);			// relative pathname
}

string FileManager::getReferenceName(size_t i, bool absolutePathname) const
{
	if (i >= lines.size())
		throw CdvaException("FileManager::getReferenceName index out of range");

	istringstream iss(lines[i]);
	string sub;
	iss >> sub;										// the query is the first name
	iss >> sub;										// the reference is the second name

	if (iss.fail())
		return string("");

	if (absolutePathname)
		return (datasetBaseDir + "/" + sub);			// absolute pathname
	else
		return (sub);			// relative pathname
}

int FileManager::countNames(size_t i) const
{
	if (i >= lines.size())
		throw CdvaException("FileManager::countNames index out of range");

	istringstream iss(lines[i]);
	string sub;
	int counter = 0;
	while (! iss.eof())
	{
		iss >> sub;
		++counter;
	}
	return counter;
}

