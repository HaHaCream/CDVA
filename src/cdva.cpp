/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Telecom Italia
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of Telecom Italia nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <limits>       // std::numeric_limits

#include "FileManager.h"
#include "LogManager.h"
#include "CdvaImpl.h"
#include "cdva.h"

using namespace std;
using namespace mpeg7cdva;

static const bool ABSOLUTE = true;
static const bool RELATIVE = false;
static const char * OPTIONS =
	      "options:\n"
	      "  -c     : generate CSV file\n"
	      "  -h     : generate HTML file\n"
	      "  -t     : produce text output\n"
	      "  -d     : dry run, try operation but make no changes\n"
	      "  -n num : process only the first num elements\n"
	      "  -w dir : set workspace directory (where descriptors are stored)\n"
	      "  -v     : verbose \n";

// ----- usage message -----
int usage()
{
	cerr <<
	"Usage: cdva <subcommand> [args] [options]\n"
	"CDVA Experimentation Model, version 1.0\n"
	"Type 'cdva help <subcommand>' for help on a specific subcommand.\n\n"
	"Available subcommands:\n"
	"  extract\n"
	"  match\n"
	"  retrieve\n\n"
	"CDVA is an acronym for 'Compact Descriptors for Video Analysis'.\n"
	"For additional information, see http://wg11.sc29.org/\n";
	return (EXIT_FAILURE);
}

int subcommand_usage(const string & subcommand)
{
	cerr <<
	"Unknown subcommand: '"<< subcommand << "'" << endl <<
	"Type 'cdva help' for usage." << endl;
	return (EXIT_FAILURE);
}

int extract_usage(const string & message)
{
	if (message.size() == 0)
		cerr << "CDVA extraction module: encode video information into a CDVA descriptor." << endl;
	else
		cerr << message << endl;

    cerr <<
      "Usage:\n"
	  "  cdva extract <vlist> <bitrate> [-k][-s][-c][-h][-t][-d][-n num][-w dir][-v]\n"
      "where:\n"
      "  vlist - text file containing the relative pathname of the video files to process (one file name per line)\n"
	  "  bitrate (0, 16, 64, 256) - bit rate of descriptors in KB/s (0 = unconstrained)\n"
  	  "specific options:\n"
  	  "  -k     : keep old descriptors (do not overwrite) if they exist\n"
  	  "  -s     : determine descriptor component sizes\n"
      << OPTIONS  << endl;

    return (EXIT_FAILURE);
}

int match_usage(const string & message)
{
	if (message.size() == 0)
		cerr << "CDVA descriptor matching module: computes the probability (score) that two videos share the same visual object" << endl;
	else
		cerr << message << endl;

    cerr <<
      "Usage:\n"
	  "  cdva match <vlist> <query_bitrate> <reference_bitrate> [-c][-h][-t][-d][-n num][-w dir][-v]\n"
      "where:\n"
      "  vlist - text file containing the relative pathname of the video files to match (two file names per line)\n"
      "  query_bitrate     (16, 64, 256) - use query descriptors of this type\n"
      "  reference_bitrate (16, 64, 256) - use reference descriptors of this type\n"
      << OPTIONS  << endl;

    return (EXIT_FAILURE);
}

int retrieve_usage(const string & message)
{
	if (message.size() == 0)
		cerr << "CDVA retrieval module." << endl;
	else
		cerr << message << endl;

    cerr <<
      "Usage:\n"
	  "  cdva retrieve <queries> <bitrate> <index> <idxrate> [-c][-h][-t][-d][-n num][-w dir][-v]\n"
      "where:\n"
      "  queries - text file containing the relative pathnames of the query video files to process (one file name per line)\n"
	  "  bitrate (16, 64, 256) - bit rate of query descriptors in KB/s\n"
      "  index - text file containing the relative pathnames of the reference descriptors (one file per line)\n"
      "  idxrate (0, 16, 64, 256) - bit rate of reference descriptors in KB/s\n"
      << OPTIONS  << endl;

    return (EXIT_FAILURE);
}

/**
 * check if a file exists and is not empty
 */
bool fexists(const char *filename)
{
  ifstream ifile(filename, ios::in | ios::binary);
  return (ifile.peek() != std::ifstream::traits_type::eof());
}

int run_extract(int argc, char *argv[])
{
	// at least 2 parameters must be given
	if (argc <= 2) {
		  return extract_usage("");
	}

	int ret = 0;

	FileManager filemanager(argv[1]);

	int bitrate = 0;
	stringstream ss(argv[2]);
	ss >> bitrate;

	// check parameters

	if (ss.fail() || (! CdvaImpl::checkBitrate(bitrate)))
		return extract_usage(string("Wrong bitrate value: ").append(argv[2]));

	// set default values

	bool verbose = false;
	bool dryrun = false;
	bool keep = false;
	bool calcSizes = false;
	int outformats = FORMAT_NONE;
	char * workspace = NULL;
	size_t limit = std::numeric_limits<size_t>::max();

	argv += 2;	// skip the first 2 params
	argc -= 2;	// skip the first 2 params

	// parse other params
	while ((argc > 1) && (argv[1][0] == '-'))
	{
		int n = 1;
		switch (argv[1][1])
		{
			case 'v': verbose = true; break;
			case 's': calcSizes = true; break;
			case 'k': keep = true; break;
			case 't': outformats |= FORMAT_TEXT; break;
			case 'c': outformats |= FORMAT_CSV; break;
			case 'h': outformats |= FORMAT_HTML; break;
			case 'n': { stringstream ss(argv[2]); ss >> limit; n = 2; break; }
			case 'w': workspace = argv[2]; n = 2; break;
			case 'd': dryrun = true; break;
			default : return extract_usage(string("wrong argument: ").append(argv[1])); break;
		}
		argv += n;
		argc -= n;
	}

	if (dryrun)
		outformats = FORMAT_TEXT;	// force text-only output in a dry run

	size_t n_videos = filemanager.getDatasetSize();
	if (n_videos > limit)
		n_videos = limit;

	filemanager.setWorkspaceDir(workspace);

	LogManager logmanager;
	logmanager.init(outformats, filemanager.getDatasetPath(), filemanager.getDatasetName(), n_videos, bitrate);

	logmanager.printExtractHeader();

	CdvaImpl cdvaimpl;								// CDVA implementation under evaluation
	cdvaimpl.init(EXTRACT, verbose, n_videos, bitrate, calcSizes);		// initialize the CDVA instance

	#pragma omp parallel for schedule(dynamic,1)
	for (int i=0; i<n_videos; i++)
	{
		string queryname = filemanager.getQueryName(i, ABSOLUTE);
		string cdva_descriptor = filemanager.replaceExt(queryname, cdvaimpl.getDescriptorExt(bitrate));

		try			// catch all errors locally
		{
			ExtractData mydata;

			if (! dryrun)
			{
				if (keep)
				{
					// check if file exists
					if (! fexists(cdva_descriptor.c_str()))
						cdvaimpl.extract(cdva_descriptor, queryname, bitrate, mydata);	// if not do it
				}
				else
					cdvaimpl.extract(cdva_descriptor, queryname, bitrate, mydata);
			}

			#pragma omp critical
			{
				logmanager.printExtractData(i, filemanager.getQueryName(i, RELATIVE), mydata);
			}
		}
		catch (exception & ex)
		{
			#pragma omp critical
			{
				cerr << "Error extracting data from video " << queryname << " : " << ex.what() << std::endl;
				ret = EXIT_FAILURE;
			}
		}
	}	// end for

	cdvaimpl.close();
	logmanager.close();
	return ret;
}


int run_match(int argc, char *argv[])
{
	// at least 3 parameters must be given
	if (argc <= 3) {
		  return match_usage("");
	}

	int ret = 0;

	FileManager filemanager(argv[1]);
	string datasetPath = filemanager.getDatasetPath();
	string videolist   = filemanager.getDatasetName();

	int bitrate = 0;
	int refbitrate = 0;
	stringstream ss(argv[2]);
	ss >> bitrate;
	stringstream refss(argv[3]);
	refss >> refbitrate;

	// check parameters

	if (ss.fail() || (! CdvaImpl::checkBitrate(bitrate)))
		return match_usage(string("Wrong query bitrate value: ").append(argv[2]));

	if (refss.fail() || (! CdvaImpl::checkBitrate(refbitrate)))
		return match_usage(string("Wrong reference bitrate value: ").append(argv[3]));

	// set default values

	bool verbose = false;
	int outformats = FORMAT_NONE;
	bool dryrun = false;
	char * workspace = NULL;
	size_t limit = std::numeric_limits<size_t>::max();

	argv += 3;	// skip the first 3 params
	argc -= 3;	// skip the first 3 params

	// parse other params
	while ((argc > 1) && (argv[1][0] == '-'))
	{
		int n = 1;
		switch (argv[1][1])
		{
			case 'v': verbose = true; break;
			case 't': outformats |= FORMAT_TEXT; break;
			case 'c': outformats |= FORMAT_CSV; break;
			case 'h': outformats |= FORMAT_HTML; break;
			case 'n': { stringstream ss(argv[2]); ss >> limit; n = 2; break; }
			case 'w': workspace = argv[2]; n = 2; break;
			case 'd': dryrun = true; break;
			default : return match_usage(string("wrong argument: ").append(argv[1])); break;
		}
		argv += n;
		argc -= n;
	}

	if (dryrun)
		outformats = FORMAT_TEXT;	// force text-only output in a dry run

	size_t n_videos = filemanager.getDatasetSize();
	if (n_videos > limit)
		n_videos = limit;

	filemanager.setWorkspaceDir(workspace);

	LogManager logmanager;
	logmanager.init(outformats, datasetPath, videolist, n_videos, bitrate, refbitrate);

	logmanager.printMatchHeader();

	CdvaImpl cdvaimpl;						// CDVA implementation under evaluation
	cdvaimpl.init(MATCH, verbose, n_videos, bitrate, refbitrate);					// initialize the CDVA instance

//	#pragma omp parallel for
	for (int i=0; i<n_videos; i++)
	{
		string query_name = filemanager.getQueryName(i);
		string ref_name = filemanager.getReferenceName(i);
		string q_fname = filemanager.replaceExt(query_name, cdvaimpl.getDescriptorExt(bitrate));
		string r_fname = filemanager.replaceExt(ref_name, cdvaimpl.getDescriptorExt(refbitrate));

		try			// catch all errors locally
		{
			MatchData matchResults;

			if (! dryrun)
				cdvaimpl.match(matchResults, q_fname, r_fname, bitrate, refbitrate);

//			#pragma omp critical
			{
				logmanager.printMatchData(i,filemanager.getQueryName(i, RELATIVE), filemanager.getReferenceName(i, RELATIVE), matchResults);
			}
		}
		catch (exception & ex)
		{
//			#pragma omp critical
			{
				cerr << "Error matching videos " << query_name << " and " << ref_name << " : " << ex.what() << std::endl;
				ret = EXIT_FAILURE;
			}
		}
	}

	cdvaimpl.close();
	logmanager.close();
	return ret;
}

int run_retrieve(int argc, char *argv[])
{
	// at least 4 parameters must be given
	if (argc <= 4) {
		  return retrieve_usage("");
	}

	int ret = 0;

	FileManager filemanager(argv[1]);		// read the annotation file

	int bitrate = 0;
	stringstream ss(argv[2]);
	ss >> bitrate;

	FileManager database(argv[3]);

	int dbbitrate = 0;
	stringstream ssdb(argv[4]);
	ssdb >> dbbitrate;

	// check parameters

	if (ss.fail() || (! CdvaImpl::checkBitrate(bitrate)))
		return retrieve_usage(string("Wrong query descriptors bit rate (KB/s): ").append(argv[2]));

	if (ssdb.fail() || (! CdvaImpl::checkBitrate(dbbitrate)))
		return retrieve_usage(string("Wrong reference descriptors bit rate (KB/s): ").append(argv[4]));

	// set default values

	bool verbose = false;
	int outformats = FORMAT_NONE;
	bool dryrun = false;
	char * workspace = NULL;
	size_t limit = std::numeric_limits<size_t>::max();

	argv += 4;	// skip the first 4 params
	argc -= 4;	// skip the first 4 params

	// parse other params
	while ((argc > 1) && (argv[1][0] == '-'))
	{
		int n = 1;
		switch (argv[1][1])
		{
			case 'v': verbose = true; break;
			case 't': outformats |= FORMAT_TEXT; break;
			case 'c': outformats |= FORMAT_CSV; break;
			case 'h': outformats |= FORMAT_HTML; break;
			case 'n': { stringstream ss(argv[2]); ss >> limit; n = 2; break; }
			case 'w': workspace = argv[2]; n = 2; break;
			case 'd': dryrun = true; break;
			default : return retrieve_usage(string("wrong argument: ").append(argv[1])); break;
		}
		argv += n;
		argc -= n;
	}

	if (dryrun)
		outformats = FORMAT_TEXT;	// force text-only output in a dry run

	size_t n_queries = filemanager.getDatasetSize();
	if (n_queries > limit)
		n_queries = limit;

	size_t n_references = database.getDatasetSize();

	filemanager.setWorkspaceDir(workspace);
	database.setWorkspaceDir(workspace);

	LogManager logmanager;
	logmanager.init(outformats, filemanager.getDatasetPath(), filemanager.getDatasetName(), n_queries, bitrate);
	logmanager.printRetrievalHeader();

	CdvaImpl cdvaimpl;									// CDVA implementation under evaluation
	cdvaimpl.init(RETRIEVE, verbose, n_references, bitrate);			// initialize the CDVA instance

	// read the index database of reference videos

	for (int i=0; i<n_references; i++)
	{
		string absolutepathname	= database.getQueryName(i, ABSOLUTE);
		string relativepathname = database.getQueryName(i, RELATIVE);
		string cdva_descriptor = database.replaceExt(absolutepathname, cdvaimpl.getDescriptorExt(dbbitrate));

		try			// catch all errors locally
		{
			if (!dryrun)
				cdvaimpl.makeindex(cdva_descriptor, relativepathname);
		}
		catch (exception & ex)
		{
				cerr << "Warning reading reference descriptor " << relativepathname << " : " << ex.what() << std::endl;
			    ret = EXIT_FAILURE;
		}
	}

	cdvaimpl.commitDB();


	// now read the queries and perform retrieval
	#pragma omp parallel for ordered schedule(dynamic)
	for (int i=0; i<n_queries; i++)
	{
		string absolutepathname	= filemanager.getQueryName(i, ABSOLUTE);
		string relativepathname = filemanager.getQueryName(i, RELATIVE);
		string q_fname = filemanager.replaceExt(absolutepathname, cdvaimpl.getDescriptorExt(bitrate));

		try			// catch all errors locally
		{
			vector<MatchData> retrievalResults;

			if (!dryrun)
				cdvaimpl.retrieve(retrievalResults, q_fname, bitrate);

			#pragma omp ordered
			{
				logmanager.printRetrievalData(i, relativepathname, retrievalResults);
			}
		}
		catch (exception & ex)
		{
			#pragma omp critical
			{
				cerr << "Error retrieving video query " << absolutepathname << " : " << ex.what() << std::endl;
			    ret = EXIT_FAILURE;
			}
		}
	}

	cdvaimpl.close();
	logmanager.close();
	return ret;
}


//  ----- main ------

int main(int argc, char *argv[])
{
	int ret = 0;

	// at least 1 parameter must be given (e.g. "help")
	if (argc <= 1) {
		  return usage();
	}

	try
	{
		string subcommand(argv[1]);
		if (subcommand.compare("help") == 0)
		{
			if (argc >= 3)		// provide help on subcommand
			{
				string operation(argv[2]);
				if (operation.compare("extract") == 0)
					return extract_usage("");
				else if (operation.compare("match") == 0)
					return match_usage("");
				else if (operation.compare("retrieve") == 0)
					return retrieve_usage("");
				else
					return subcommand_usage(operation);
			}
			else
				return usage();
		}

		argv += 1;
		argc -= 1;

		if (subcommand.compare("extract") == 0)
			return run_extract(argc, argv);			// run "extract" catching any exception
		else if (subcommand.compare("match") == 0)
			return run_match(argc, argv);			// run "match" catching any exception
		else if (subcommand.compare("retrieve") == 0)
			return run_retrieve(argc, argv);		// run "retrieve" catching any exception
		else
			return subcommand_usage(subcommand);
	}
	catch(exception & ex)				// catch any exception, including CdvaException
	{
	    cerr << argv[0] << " exception: " << ex.what() << endl;
	    ret = EXIT_FAILURE;
	}

	return ret;
}

// end of main

