/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CdvaException.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <cstdlib>
#include "CSVReader.h"

using namespace std;
using namespace mpeg7cdva;

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


/*
	----- structure of Ground truth is the following ----------
id, queryID, matchingReferenceID
1,EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4,EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5897.MOV.mp4
1,EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4,EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5898.MOV.mp4
1,EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4,EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5899.MOV.mp4

 -------- timing ground truth ---------
 id, videoID, type, start, end
1,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0947.MOV,2,29,32.63
2,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0948.MOV,1,20,26.92
3,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0949.MOV,1,13,19.8

	----- struture of Retrieval results is the following --------
id, queryID, matchingReferenceID, score, firstMatchTime, lastMatchTime
1, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5897.MOV.mp4, 76.861, 28.523, 28.899
1, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5898.MOV.mp4, 28.819, 28.523, 33.778
1, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5901.MOV.mp4, 26.3206, 28.523, 33.778
1, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0001_I00001_C001/EMB02_ORG_MVI_5902.MOV.mp4, 21.8348, 28.523, 33.778

   ------ structure of queries and references ------
id, pathname, nframes, time, fps, descriptorSize
9, EMB01_ORG/S0002_I00001_C001/EMB01_ORG_cwi_centrum_wiskunde_informatica_clip3.mp4.mp4, 958, 32.1584, 29.79, 51442
5, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5901.MOV.mp4, 888, 29.6296, 29.97, 22874
32, EMB01_ORG/S0002_I00004_C001/EMB01_ORG_cwi_universiteit_van_amsterdam_clip1.mp4.mp4, 1434, 48.7258, 29.43, 41661
6, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5902.MOV.mp4, 1264, 42.1755, 29.97, 26353

   ------ structure of matching and non-matching pairs -------
id, queryName, referenceName, score, firstMatchTime, lastMatchTime
1, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0015_I00031_C001/Calabria_Le_Castella_8.wmv, 15.5221, 33.402, 33.778
2, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0021_I00030_C002/EMB02_ORG_MVI_1727.MP4.mp4, 0, -1, -1
3, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0018_I00162_C001/EMB02_ORG_object162.shot07.mp4.mp4, 0, -1, -1
4, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5897.MOV.mp4, EMB02_ORG/S0015_I00148_C001/Puglia_Teatro_Petruzzelli_5.mp4, 0, -1, -1

*/
/*
  -------- timing ground truth ---------
 id, videoID, type, start, end
1,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0947.MOV,2,29,32.63
2,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0948.MOV,1,20,26.92
3,EMB01_CAM/S0001_I00001_C001/EMB01_CAM_IMG_0949.MOV,1,13,19.8
*/
int CSVReader::read(timing_map_t & map) const
{
	int maxID = 0;

	timing_t data;

	size_t num = getDatasetSize();
	for (size_t i=1; i<num; ++i)
	{
		string myline = getLine(i);

		stringstream ss(myline);
		string single;

		getline(ss, single, ',');
		string id = trim(single);
		getline(ss, single, ',');
		string queryName = trim(single);
		getline(ss, single, ',');
		string type = trim(single);
		getline(ss, single, ',');
		string start = trim(single);
		getline(ss, single, ',');
		string end = trim(single);

		int myID = atoi(id.c_str()) - 1;

		if (myID > maxID)
			maxID = myID;

		// data.queryName = queryName;
		data.type = atoi(type.c_str());
		data.start = atof(start.c_str());
		data.end = atof(end.c_str());
		map[queryName] = data;		// insert data in map
	}
	return (maxID + 1);		// return the number of items
}

size_t CSVReader::getNumRecords() const
{
	int maxID = 0;
	size_t num = getDatasetSize();
	size_t lastquarter = (num * 3)/4;
	if (lastquarter == 0)
		return 0;

	// compute the maximum id (supposing that the maximum id is in the last positions)

	for (size_t i = lastquarter; i<num; ++i)
	{
		string match = getLine(i);

		stringstream ss(match);
		string single;

		getline(ss, single, ',');
		string id = trim(single);
		int myID = atoi(id.c_str()) - 1;

		if (myID > maxID)
			maxID = myID;
	}

	return (maxID + 1);
}

//	id, queryName, referenceName, score, firstMatchTime, lastMatchTime
int CSVReader::read(vector<pair_t> & data) const
{

	size_t num = getDatasetSize();
	data.resize(getNumRecords());		// the actual size is computed in getNumRecords

	for (size_t i=1; i<num; ++i)
	{
		string match = getLine(i);

		stringstream ss(match);
		string single;

		getline(ss, single, ',');
		string id = trim(single);
		getline(ss, single, ',');
		string queryName = trim(single);
		getline(ss, single, ',');
		string referenceName = trim(single);
		getline(ss, single, ',');
		string score = trim(single);
		getline(ss, single, ',');
		string firstMatchTime = trim(single);
		getline(ss, single, ',');
		string lastMatchTime = trim(single);

		int myID = atoi(id.c_str()) - 1;

		data[myID].queryName = queryName;
		data[myID].referenceName = referenceName;
		data[myID].score = atof(score.c_str());
		data[myID].firstMatchTime = atof(firstMatchTime.c_str());
		data[myID].lastMatchTime = atof(lastMatchTime.c_str());
	}

	return (int) data.size();		// return the number of items
}

/*
 read query and reference CSV files; format is the following:
 id, pathname, nframes, time, fps, descriptorSize
 5, EMB01_ORG/S0001_I00001_C001/EMB01_ORG_MVI_5901.MOV.mp4, 888, 29.6296, 29.97, 22874
 */
int CSVReader::read(vector<descriptor_t> & data) const
{
	size_t num = getDatasetSize();
	data.resize(getNumRecords());		// the actual size is computed in getNumRecords

	for (size_t i=1; i<num; ++i)
	{
		string match = getLine(i);

		stringstream ss(match);
		string single;

		getline(ss, single, ',');
		string id = trim(single);
		getline(ss, single, ',');
		string pathname = trim(single);
		getline(ss, single, ',');
		string nframes = trim(single);
		getline(ss, single, ',');
		string time = trim(single);
		getline(ss, single, ',');
		string fps = trim(single);
		getline(ss, single, ',');
		string descriptorSize = trim(single);

		int myID = atoi(id.c_str()) - 1;

		data[myID].pathname = pathname;
		data[myID].nframes = atoi(nframes.c_str());
		data[myID].time = atof(time.c_str());
		data[myID].fps = atof(fps.c_str());
		data[myID].descriptorSize = atof(descriptorSize.c_str());
	}

	return (int) data.size();		// return the number of items
}

int CSVReader::read(vector<retrieval_t> & data) const
{
	size_t num = getDatasetSize();
	data.resize(getNumRecords());		// the actual size is computed in getNumRecords

	for (size_t i=1; i<num; ++i)
	{
		string match = getLine(i);

		stringstream ss(match);
		string single;

		getline(ss, single, ',');
		string id = trim(single);
		getline(ss, single, ',');
		string query = trim(single);
		getline(ss, single, ',');
		string reference = trim(single);

		 int myID = atoi(id.c_str()) - 1;

		 if (data[myID].n_matches == 0) // this is the first match
		 {
			 query.copy(data[myID].query, query.length());
			 data[myID].query[query.length()]='\0';
		 }
		 int k = data[myID].n_matches++ ;
		 if (k < MAX_MATCHES)
		 {
			 reference.copy(data[myID].matches[k], reference.length());
			 data[myID].matches[k][reference.length()]='\0';
		 }
	}

	return (int) data.size();		// return the number of items
}





CSVReader::CSVReader(const char *filepathname, int level):lines(),irlevel(level),hasWorkspaceDir(false)
{
	readAnnotation(filepathname);
}

CSVReader::~CSVReader() {
}

string CSVReader::getDatasetPathName() const
{
	return datasetBaseDir + "/" + datasetName;
}

string CSVReader::getDatasetPath() const
{
	return datasetBaseDir;
}

string CSVReader::getDatasetName() const
{
	return datasetName;
}

size_t CSVReader::getDatasetSize() const
{
	return ds_size;
}

void CSVReader::setWorkspaceDir(const char * workdir)
{
	if (workdir != NULL)
	{
		workspaceDir = workdir;
		hasWorkspaceDir = true;
	}
}

std::string CSVReader::getWorkspaceDir() const
{
	return workspaceDir;
}



size_t CSVReader::readAnnotation(const char *filepathname)
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
		   lines.push_back( line );
	   }
   }

   ds_size = lines.size();
   return ds_size;
}

string CSVReader::replaceExt(const string & imageName, const char * ext) const
{
	string newname;
	if (hasWorkspaceDir)	// in this case, use workspaceDir in place of datasetBaseDir
		newname = workspaceDir + imageName.substr(datasetBaseDir.length());
	else
		newname = imageName;

	size_t found = newname.find_last_of('.');
	return newname.substr(0,found) + ext;
}

string CSVReader::replacePath(const std::string & imageName, const char * newpath)
{
	std::size_t found = imageName.find_last_of("/\\");
	return string(newpath) + "/" + imageName.substr(found+1);
}

string CSVReader::getLine(size_t i) const
{
	if (i >= lines.size())
		throw CdvaException("FileManager::getQueryName index out of range");

	return (lines[i]);
}


int CSVReader::countNames(size_t i) const
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

