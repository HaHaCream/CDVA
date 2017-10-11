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
#include <fstream>
#include <string>
#include <vector>
#include "cdva.h"		// needed to include MatchData declaration

namespace mpeg7cdva
{

/**
 * Format of output logs
 */
enum LogFormat {
	FORMAT_NONE = 0,	///< do not output any data
	FORMAT_CSV =  1,	///< output data in CSV format
	FORMAT_TEXT = 2,	///< output data as free text
	FORMAT_HTML = 4		///< output data in HTML format
};


/**
 * @class LogManager
 * Helper class to produce log files in various formats (csv, text, XML, etc.)
 * @author Massimo Balestri
 * @date 2015
 */
class LogManager {
private:
	int myformats;
	size_t numvideos;
	std::string mydatasetpath;
	std::string mydatasetname;
	std::ofstream outcsv;
	std::ofstream outhtml;
	static const char * getModeExtension(int qmode, int refmode);
	static const char * getFormatExtension(LogFormat format);
	static const char * getFormatName(LogFormat format);

public:
	LogManager();	///< constructor

	virtual ~LogManager();	///< destructor

	/**
	 * initialization method used by the Evaluation Framework; do not change.
	 */
	void init(int formats, const std::string & datasetpath, const std::string & datasetname, size_t n_videos, int mode, int refmode = 0);

	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printExtractHeader();
	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printExtractData(int index, const std::string & videoname, const ExtractData & data);

	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printMatchHeader();
	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printMatchData(int index, const std::string & queryvideoname, const std::string & refvideoname, const MatchData & matchData);

	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printRetrievalHeader();
	/**
	 * method used by the Evaluation Framework to produce log files; do not change.
	 */
	void printRetrievalData(int index, const std::string & queryvideoname, const std::vector<MatchData> & retrievalResults);

	void close();
};

}	// end namespace
