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

#include <string>
#include <vector>
#include <map>

#define MAX_MATCHES 50
#define MAX_FILENAME_LENGTH 256

namespace mpeg7cdva
{

typedef char FILENAME[MAX_FILENAME_LENGTH];


/**
 * Class containing the timing ground truth used in pairwise matching.
 */
class timing_t {
public:
	int type;
	double start;
	double end;

	timing_t() {
		type = 0;
		start = 0.0;
		end = 0.0;
	}

};


/**
 * Class containing the name of a query video and the list of names of matching reference videos.
 */
class retrieval_t {
public:
	FILENAME query;					///< query image
	int n_matches;					///< number of matching images
	FILENAME matches[MAX_MATCHES];	///< list of matches
	retrieval_t() {
		n_matches = 0;
		query[0] = '\0';
		for (int k=0; k<MAX_MATCHES; ++k)
			matches[k][0] = '\0';
	}
};

/**
 * Class containing the name of a query/reference descriptor video and the associated information.
 */
class descriptor_t {
public:
	std::string pathname;
	int nframes;
	double time;
	double fps;
	double descriptorSize;

	descriptor_t ():pathname() {
			nframes = 0;
			fps = 0.0;
			time = 0.0;
			descriptorSize = 0.0;
	}
};

/**
 * Class containing the name of a query video and the associated information.
 */
class pair_t {
public:
	std::string queryName;
	std::string referenceName;
	double score;
	double firstMatchTime;
	double lastMatchTime;

	pair_t(): queryName(),referenceName() {
		score = 0.0;
		firstMatchTime = 0.0;
		lastMatchTime = 0.0;
	}
};

typedef std::map<std::string, timing_t> timing_map_t;


/**
 * @class CSVReader
 * Helper class to read CSV files.
 * @author Massimo Balestri
 * @date 2016
 */
class CSVReader {
private:
	std::vector<std::string> lines;
	std::string datasetBaseDir;
	std::string datasetName;
	std::string workspaceDir;
	size_t ds_size;				// number of elements in the dataset
	int irlevel;				// include recursion level
	bool hasWorkspaceDir;

	/**
	 * Read the list of images from the given annotation file.
	 * @param filepathname the pathname of the annotation text file containing the list of images.
	 * @return the number of lines read from the filename.
	 */
	size_t readAnnotation(const char * filepathname);


public:

	/**
	 * Read the list of images from the given annotation file.
	 * @param annotationpathname the pathname of the annotation text file containing the list of images.
	 * @param level the recursion level (to avoid infinite loops); must be zero when called the first time.
	 */
	CSVReader(const char * annotationpathname, int level = 0);

	virtual ~CSVReader();

	/**
	 * Read the timing ground truth for pairwise matching.
	 * @param map the map to read
	 * @return the number of items
	 */
	int read(timing_map_t & map) const;

	/**
	 * Read the a pair list for pairwise matching.
	 * @param data the matching/non-matching pairs to read
	 * @return the number of items
	 */
	int read(std::vector<pair_t> & data) const;

	/**
	 * Read the query/reference descriptor data.
	 * @param data to read
	 * @return the number of items
	 */
	int read(std::vector<descriptor_t> & data) const;

	/**
	 * Read the retrieval data.
	 * @param data to read
	 * @return the number of items
	 */
	int read(std::vector<retrieval_t> & data) const;

	/**
	 * Get the dataset base directory.
	 * @return the dataset path
	 */
	std::string getDatasetPath() const;

	/**
	 * Get the dataset name.
	 * @return the dataset name
	 */
	std::string getDatasetName() const;

	/**
	 * Get the dataset full pathname.
	 * @return the dataset pathname
	 */
	std::string getDatasetPathName() const;

	/**
	 * Set the workspace directory. This is the directory where output files will be stored.
	 * @param workdir the workspace directory
	 */
	void setWorkspaceDir(const char * workdir);

	/**
	 * Get the workspace directory. This is the directory where output files will be stored.
	 * @return the workspace directory
	 */
	std::string getWorkspaceDir() const;


	/**
	 * Get the dataset size.
	 * @return the number of lines read from the filename.
	 */
	size_t getDatasetSize() const;

	/**
	 * Get the number of records (rows) in the CSV table
	 */
	size_t getNumRecords() const;

	/**
	 * Convert a pathname into a pathname with the given extension.
	 * @param imageName the original image/video name;
	 * @param ext new extension;
	 * @return the modified pathname.
	 */
	std::string replaceExt(const std::string & imageName, const char * ext) const;

	/**
	 * Convert a pathname into a pathname with the given new path.
	 * @param imageName the original image name;
	 * @param newpath the new path;
	 * @return the modified pathname.
	 */
	static std::string replacePath(const std::string & imageName, const char * newpath);



	/**
	 * Get the line at the i-th position in the CSV file.
	 * @param i the index of the image in the CSV file.
	 * @return the CSV line.
	 */
	std::string getLine(size_t i) const;

	/**
	 * Count how many pathnames are contained in the i-th line.
	 * @param i the index of the image in the annotation file.
	 * @return the number of pathnames found.
	 */
	int countNames(size_t i) const;
};

}	// end namespace
