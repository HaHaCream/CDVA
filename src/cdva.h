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

/**
@mainpage Documentation

This is the documentation of the C++ classes implementing the MPEG CDVA Experimentation Model (CXM).
The software implements the recommendations contained in the following documents (at http://wg11.sc29.org/):
	- N15338:"Evaluation Framework for Compact Descriptors for Video Analysis - Search and Retrieval", July 2015, Warsaw, Poland
	- N15729:"Evaluation Framework for Compact Descriptors for Video Analysis - Search and Retrieval – Version 2.0", October 2015, Geneva, CH

Documentation on how to build and install the code is contained in the "CDVA_build_run_instructions" document which can be found in the "docs" directory.
*/


/**
 * @namespace mpeg7cdva
 * Namespace used to encapsulate all MPEG-7 CDVA declarations.
 */
namespace mpeg7cdva
{

/**
 * @class ExtractData
 * A class containing the results of an extraction operation.
 */
class ExtractData {
public:
	double descriptorlength;
	double clip_duration;
	double numframes;
	double numshots;

	// counters - required by CDVA core experiments

	int header_bit_count;
	int local_bit_count;
	int global_bit_count;
	int coordinate_bit_count;
	int n_keyframes;

	ExtractData ()
	{
		clip_duration = 0;
		descriptorlength = 0;
		numframes = 0;
		numshots = 0;
		header_bit_count =
		local_bit_count =
		global_bit_count =
		coordinate_bit_count =
		n_keyframes = 0;
	}

	/**
	 * set the video duration in seconds.
	 * @param time the time in seconds
	 */
	void setVideoDuration(double time)
	{
		clip_duration = time;
	}

	/**
	 * set the number of frames of the video clip.
	 * @param nframes number of frames of the video clip.
	 */
	void setNumFrames(double nframes)
	{
		numframes = nframes;
	}

	/**
	 * set the number of shots of the video clip.
	 * @param nshots number of shots of the video clip.
	 */
	void setNumShots(double nshots)
	{
		numshots = nshots;
	}


	/**
	 * set the actual descriptor length (in bytes).
	 * @param length the size in bytes of the encoded descriptor.
	 */
	void setDescriptorLength(double length)
	{
		descriptorlength = length;
	}

};

/**
 * @class MatchData
 * A class containing the results of a matching or retrieval operation.
 */
class MatchData {
private:
	double score;					// score of matching
	std::string reference_ID;		// identifier of the matching reference video
	double startTime;				// time in seconds indicating the fist matching frame of the query clip (e.g. 3.456 s)
	double endTime;					// time in seconds indicating the last matching frame of the query clip (e.g. 6.352 s)

public:
	MatchData()
	{
		score = 0;
		startTime = endTime = -1;
	}

	virtual ~MatchData()
	{}


	/**
	 * set the score of matching the query image with the reference image.
	 * @param myscore the overall matching score
	 */
	void setMatchingScore(double myscore)
	{
		score = myscore;		// save score
	}

	/**
	 * set the time of each frame matching (only the first and the last will be saved).
	 * @param time_s the time in seconds from the start of the query video sequence
	 */
	void setMatchingTime(double time_s)
	{
		if (startTime < 0)
		{
			startTime = endTime = time_s;		// this is the first frame matching
		}
		else
		{
			if (time_s > endTime)
				endTime = time_s;

			if (time_s < startTime)
				startTime = time_s;
		}
	}

	/**
	 * set the string that identifies the matching reference video clip.
	 * @param reference the identifier (usually the relative pathname) of the matching reference video.
	 */
	void setReferenceID(const std::string reference)
	{
		reference_ID = reference;
	}

	/**
	 * Get the matching score.
	 * @return the score
	 */
	double getScore() const
	{
		return score;
	}

	/**
	 * get the time in seconds indicating the fist matching frame of the query clip.
	 * @return the time in seconds from the start of the video clip
	 */
	double getFirstMatchingTime() const
	{
		return startTime;
	}

	/**
	 * get the time in seconds indicating the last matching frame of the query clip.
	 * @return the time in seconds from the start of the video clip
	 */
	double getLastMatchingTime() const
	{
		return endTime;
	}

	/**
	 * get the string that identifies the matching reference video clip.
	 * @return the video clip relative pathname
	 */
	std::string getReferenceId() const
	{
		return reference_ID;
	}

};

}	// end of namespace
