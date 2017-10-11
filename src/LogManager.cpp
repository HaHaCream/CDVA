/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LogManager.h"
#include <iostream>

using namespace mpeg7cdva;
using namespace std;

LogManager::LogManager() {
	myformats = FORMAT_NONE;		// default
	numvideos = 0;
}

LogManager::~LogManager() {};

const char * LogManager::getModeExtension(int qmode, int refmode)
{
	if (refmode > 0)
	{
		int mode = 1000*qmode + refmode;
		switch (mode)
		{
			case 16016:  return ".16K";
			case 16064:  return ".16K_64K";
			case 16256:  return ".16K_256K";
			case 64016:  return ".64K_16K";
			case 64064:  return ".64K";
			case 64256:  return ".64K_256K";
			case 256016: return ".256K_16K";
			case 256064: return ".256K_64K";
			case 256256: return ".256K";
		}
	}
	else
	{
		switch (qmode)
		{
			case   0: return ".DB";		// variable length mode (can be used in descriptors that are stored in the local index, i.e. the "data base")
			case  16: return ".16K";
			case  64: return ".64K";
			case 256: return ".256K";
		}
	}
	return NULL;
}

const char * LogManager::getFormatExtension(LogFormat format)
{
	switch (format)
	{
		case FORMAT_TEXT: return ".log";
		case FORMAT_CSV: return ".csv";
		case FORMAT_HTML: return ".html";
	}
	return NULL;
}

const char * LogManager::getFormatName(LogFormat format)
{
	switch (format)
	{
		case FORMAT_TEXT: return "TEXT";
		case FORMAT_CSV:  return "CSV";
		case FORMAT_HTML: return "HTML";
	}
	return NULL;
}

void LogManager::init(int formats, const string & datasetpath, const string & datasetname, size_t n_videos, int mode, int refmode)
{
	myformats = formats;
	mydatasetname = datasetname;
	mydatasetpath = datasetpath;
	numvideos = n_videos;

	if (formats & FORMAT_TEXT)
	{
		cout << "output format: " << getFormatName(FORMAT_TEXT) << "; printing to standard output. " << endl;
	}

	if (formats & FORMAT_CSV)
	{
		size_t found = datasetname.find_last_of('.');
		string name = datasetname.substr(0,found) + getModeExtension(mode, refmode) + getFormatExtension(FORMAT_CSV);
		outcsv.open(name.c_str());
		cout << "output format: " << getFormatName(FORMAT_CSV) << "; output log file: " << name << endl;
	}

	if (formats & FORMAT_HTML)
	{
		size_t found = datasetname.find_last_of('.');
		string name = datasetname.substr(0,found) + getModeExtension(mode, refmode) + getFormatExtension(FORMAT_HTML);
		outhtml.open(name.c_str());
		cout << "output format: " << getFormatName(FORMAT_HTML) << "; output log file: " << name << endl;

		// in case of HTML we need a header

		outhtml <<
			"<!DOCTYPE html>\n"
			"<html>\n"
			"<head>\n"
			"<style>\n"
			"table, th, td {\n"
			"    border: 1px solid black;\n"
			"    border-collapse: collapse;\n"
			"}\n"
			"th, td {\n"
			"    padding: 5px;\n"
			"    text-align: left;\n"
			"}\n"
			"th {\n"
			" background-color: #f1f1c1;\n"
			"}\n"
			"</style>\n"
			"</head>\n"
			"<body>\n"
			"<table>"
		<< endl;
	}
}

void LogManager::close()
{
	if (myformats & FORMAT_HTML )
	{
		outhtml <<
			"</table>\n"
			"</body>\n"
			"</html>"
		<< endl;
	}

	 if (outcsv.is_open())
		 outcsv.close();

	 if (outhtml.is_open())
		 outhtml.close();
}


void LogManager::printExtractHeader()
{
	if (myformats & FORMAT_CSV)
	{
		outcsv << "id, pathname, nframes, time, fps, descriptorSize, headerSize, globalSize, localSize, coordinateSize, numKeyFrames, numShots " << endl;
	}

	if (myformats & FORMAT_HTML)
	{
		outhtml << "<tr><th>id</th><th>pathname</th><th>nframes</th><th>time</th><th>fps</th><th>descriptor size</th><th>num of key frames</th><th>num of shots</th></tr>" << endl;
	}
}

void LogManager::printExtractData(int index, const string & videoname, const ExtractData & data)
{
	double fps = 0;
	double byteps = 0;
	if (data.clip_duration > 0)
	{
		fps = data.numframes/data.clip_duration;
		byteps = data.descriptorlength / data.clip_duration;
	}

	if (myformats & FORMAT_TEXT)
	{
		cout << "[" << index + 1 << "/" << numvideos << "]: "
			 << videoname << " (" << data.numframes << " frames, " << data.clip_duration << " s, " << fps << " fps)"
			 << " -> " << data.descriptorlength << " [bytes], "<< byteps << " [byte/s], " << data.n_keyframes << " [key frames], " << data.numshots << " [shots]" << endl;
	}
	if (myformats & FORMAT_CSV)
	{
		outcsv  << index + 1 << ", "
			 << videoname << ", " << data.numframes << ", " << data.clip_duration << ", " << fps << ", "
			 << data.descriptorlength << ", " << data.header_bit_count / 8 << ", " << data.global_bit_count  / 8 << ", "
			 << data.local_bit_count  / 8 << ", " << data.coordinate_bit_count  / 8 << ", " << data.n_keyframes << ", " << data.numshots << endl;
	}
	if (myformats & FORMAT_HTML)
	{
		outhtml << "<tr><td>" << index + 1 << "</td><td>"  << videoname << "</td><td>" << data.numframes << "</td><td>" << data.clip_duration << "</td><td>" << fps
			<< "</td><td>" << data.descriptorlength <<  "</td><td>" << data.n_keyframes <<  "</td><td>"  << data.numshots << "</td></tr>" << endl;
	}
}

// match

void LogManager::printMatchHeader()
{
	if (myformats &  FORMAT_CSV)
	{
		outcsv << "id, queryName, referenceName, score, firstMatchTime, lastMatchTime" << endl;
	}

	if (myformats & FORMAT_HTML)
	{
		outhtml << "<tr><th>id</th><th>query name</th><th>reference name</th><th>score</th><th>first match time (s)</th><th>last match time (s)</th></tr>" << endl;
	}
}

void LogManager::printMatchData(int index, const std::string & queryvideoname, const std::string & refvideoname, const MatchData & matchData)
{
	if (myformats & FORMAT_TEXT)
	{
		cout << "[" << index + 1 << "/" << numvideos << "]: " << queryvideoname << " vs "<< refvideoname
			 << " -> " << matchData.getScore() << " [score], " << matchData.getFirstMatchingTime() << " [s], " << matchData.getLastMatchingTime() << " [s]" << endl;
	}
	if (myformats & FORMAT_CSV)
	{
		outcsv  << index + 1 << ", "
			 << queryvideoname << ", " << refvideoname << ", "
			 << matchData.getScore() << ", " << matchData.getFirstMatchingTime() << ", " << matchData.getLastMatchingTime() << endl;
	}
	if (myformats & FORMAT_HTML)
	{
		outhtml << "<tr><td>" << index + 1 << "</td><td>"  << queryvideoname << "</td><td>" << refvideoname << "</td><td>"
			<< matchData.getScore() << "</td><td>" <<matchData.getFirstMatchingTime() << "</td><td>" << matchData.getLastMatchingTime()<< "</td></tr>" << endl;
	}
}

void LogManager::printRetrievalHeader()
{
	if (myformats & FORMAT_CSV)
	{
		outcsv << "id, queryID, matchingReferenceID, score, firstMatchTime, lastMatchTime" << endl;
	}

	if (myformats & FORMAT_HTML)
	{
		outhtml << "<tr><th>id</th><th>query ID</th><th>matching reference ID</th><th>score</th><th>first match time (s)</th><th>last match time (s)</th></tr>" << endl;
	}
}


void LogManager::printRetrievalData(int index, const string & queryvideoname, const vector<MatchData> & mdvector)
{
	if (myformats & FORMAT_TEXT)
	{
		cout << "[" << index + 1 << "/" << numvideos << "]: " << queryvideoname ;
		for (size_t k=0; k<mdvector.size(); ++k)
		{
			 cout << ", " << mdvector[k].getReferenceId();
		}
		cout << endl;
	}
	if (myformats & FORMAT_CSV)
	{
		for (size_t k=0; k<mdvector.size(); ++k)
		{
			outcsv  << index + 1 << ", "
				 << queryvideoname << ", " << mdvector[k].getReferenceId() << ", "
				 << mdvector[k].getScore() << ", " << mdvector[k].getFirstMatchingTime() << ", " << mdvector[k].getLastMatchingTime() << endl;
		}
	}
	if (myformats & FORMAT_HTML)
	{
		for (size_t k=0; k<mdvector.size(); ++k)
		{
			outhtml << "<tr><td>" << index + 1 << "</td><td>"  << queryvideoname << "</td><td>" << mdvector[k].getReferenceId() << "</td><td>"
				<< mdvector[k].getScore() << "</td><td>" << mdvector[k].getFirstMatchingTime() << "</td><td>" << mdvector[k].getLastMatchingTime() << "</td></tr>" << endl;
		}
	}
}

// end LogManager
