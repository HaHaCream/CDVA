/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Joanneum Research, Mitsubishi Electric Research Labs, Peking University, Telecom Italia, University of Surrey, Visual Atoms

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the names of Joanneum Research, Mitsubishi Electric Research Labs, Peking University, Telecom Italia, University of Surrey, Visual Atoms nor the names of their contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <string>
#include <vector>
#include <map>

#include "cdva.h"
#include "CdvaException.h"
#include "LogManager.h"
#include "Buffer.h"
#include "CdvsInterface.h"


namespace mpeg7cdva
{

	enum OPERATION { UNKNOWN, EXTRACT, MATCH, RETRIEVE };
	
	/**
	 * @class SegmentDescriptor
	 * A container for CdvsDescriptor instances belonging to the same video segment.
	 */
	class SegmentDescriptor
	{
	private:
		unsigned long startTime;
		unsigned long endTime;
		unsigned int parity;

		static const int SHOT_HEADER_SIZE = 10;		// header size in bytes

		size_t getSize() const;		// get the size in bytes of encoded keyframes when written into a buffer

	public:
		std::vector <mpeg7cdvs::CdvsDescriptor> keyframes;	///< keyframes belonging to this shot

		void setParity(unsigned int value);					///< Set the shot parity
		unsigned int getParity() const;						///< Get the shot parity
		void setStartTimeMs(unsigned long position_ms);		///< Set the start time of the shot in milliseconds
		void setEndTimeMs(unsigned long position_ms);		///< Set the start time of the frame in milliseconds
		unsigned long getStartTimeMs() const;				///< Get the shot start time in milliseconds
		unsigned long getEndTimeMs() const;					///< Get the frame start time in milliseconds
		size_t write(std::ofstream & fout) const;			///< write (appending) this segment to a file
		size_t read(std::ifstream & fin, mpeg7cdvs::CdvsServer * cdvsserver);		///< read (from the current position) this segment from a file
		static size_t addSegmentToDB(std::ifstream & fin, mpeg7cdvs::CdvsServer * cdvsserver, const std::string & relativepathname); ///< read (from the current position) this segment from a file and store it into the CdvsServer DB
		bool empty() const;				///< returns true if the segment is empty
		void clear();					///< clear the shot container
	};



	typedef std::vector<SegmentDescriptor> ShotDescriptorList;		///< vector of shots descriptors of a video





	/**
	* @class CdvaImpl
	* A CDVA implementation based on multiple CDVS descriptors
	*/
	class CdvaImpl 
	{
	private:
		void updateCounters(const  mpeg7cdvs::CdvsDescriptor & desc, int total_bit_count, ExtractData & outdata) const;
	protected:
		mpeg7cdvs::CdvsConfiguration * cdvsconfig;
		mpeg7cdvs::CdvsClient * cdvsclient;
		mpeg7cdvs::CdvsServer * cdvsserver;

		bool verboseMode;		///< verbose mode indicator
		OPERATION current_op;	///< the current operation
		int cdvsMode;			///< the CDVS mode that will be used to encode keyframe descriptors
		int skip_before;		///< number of video frames to skip before decoding one
		int skip_after;			///< number of video frames to skip after decoding one
		double drop_frame_th;	///< drop frame threshold
		double shot_cut_th;		///< shot cut threshold
		double shot_ver_th;		///< shot verification threshold
		size_t max_retrieved;	///< maximum number of retrieved images
		bool calc_desc_sizes;   ///< calculate size of descriptor components

		static bool byDescendingScore(const MatchData & m1, const MatchData & m2);	// order data by descending score

		virtual void parse(const std::string & descFile, ShotDescriptorList& shotList);

		virtual double match(MatchData & matchResults, const ShotDescriptorList& qDescList, const ShotDescriptorList& rDescList);


		/*
		 * Get the extension of a file name.
		 * @param imageName the original image/video name;
		 * @return the extension of the name;
		 */
		static std::string getExt(const std::string & imageName);


	public:
		/**
		 * check if the given bitrate is one of the standard values defined in the CDVA evaluation framework.
		 * @param bitrate the bitrate in Kilo-byte per second (KB/s)
		 * @return true if valid
		 */
		static bool checkBitrate(int bitrate);

		/**
		* get the file extension corresponding to the given bitrate.
		* @param bitrate the bitrate in Kilo-byte per second (KB/s)
		* @return the file extension
		*/
		static const char * getDescriptorExt(int bitrate);

		CdvaImpl();
		virtual ~CdvaImpl();

		/**
		* initialization method - called once before processing videos.
		* @param op one of EXTRACT, MATCH, RETRIEVE
		* @param verbose when set, more information is provided
		* @param n_videos the number of videos to be processed
		* @param querybitrate the query encoding bitrate (one of 0,16,64,256)
		* @param refbitrate the reference encoding bitrate (one of 0,16,64,256)
		* @param calcdescsizes if true, the size of individual elements of the CDVS descriptor is reported in the output log files
		*/
		virtual void init(OPERATION op, bool verbose, size_t n_videos, int querybitrate, int refbitrate = 0, bool calcdescsizes=false);

		/**
		 * Video processing method - called once for each video in the list.
		 * @param descrname output descriptor pathname
		 * @param videopathname input video stream pathname
		 * @param bitrate encoding bitrate (one of 0,16,64,256)
		 * @param outdata the container for output data
		 */
		void extract(const std::string & descrname, const std::string & videopathname, int bitrate, ExtractData & outdata) const;

		/**
		* Video matching method -  called once for each pair of videos in the list.
		* @param matchResults container for the results of matching
		* @param qdescrname input query descriptor name
		* @param rdescrname input reference descriptor name
		* @param qbitrate query bitrate (one of 16,64,256)
		* @param rbitrate reference bitrate (one of 16,64,256)
		* @return the matching score (normalized in the [0..1] range)
		*/
		virtual double match(MatchData & matchResults, const std::string & qdescrname, const std::string & rdescrname, int qbitrate, int rbitrate);

		/**
		* Video indexing method - builds a DB of reference video descriptors.
		* @param cdva_descriptor the descriptor to add to the DB
		* @param relativepathname the relative pathname of the video file to be used as unique identifier
		*/
		virtual void makeindex(const std::string & cdva_descriptor, const std::string & relativepathname);

		/**
		* Video retrieval method - returns a list of reference videos matching the query video.
		* @param retrievalResults the output vector containing an ordered list of matching reference videos
		* @param qdescrname the video query descriptor
		* @param qbitrate query bitrate (one of 16,64,256)
		*/
		virtual void retrieve(std::vector<MatchData> & retrievalResults, const std::string & qdescrname, int qbitrate);

		/**
		 * Save database information.
		 */
		void commitDB();

		/**
		* de-initialization method - called once at the end of processing.
		*/
		virtual void close();

	};	// end class CDVAImpl

}  // end of namespace mpeg7cdva
