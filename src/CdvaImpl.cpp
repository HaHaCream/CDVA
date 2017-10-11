/*
The copyright in this software is being made available under this MPEG Reference Software Copyright License. This software may be subject to other third party and contributor rights, including patent rights, and no such rights are granted under this license.

Copyright (c) 2016, Joanneum Research, Mitsubishi Electric Research Labs, Peking University, Telecom Italia, University of Surrey, Visual Atoms

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the names of Joanneum Research, Mitsubishi Electric Research Labs, Peking University, Telecom Italia, University of Surrey, Visual Atoms nor the names of their contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "CdvaImpl.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace cv;
using namespace mpeg7cdva;
using namespace mpeg7cdvs;
using namespace std;

#define FOURCC_MP43 0x3334504D		// identify MS codec MP43



///////////////////////////////// class ShotDescriptor //////////////////////////////////
bool SegmentDescriptor::empty() const
{
	return (keyframes.size() == 0);
}

void SegmentDescriptor::clear()
{
	keyframes.clear();
}


void SegmentDescriptor::setStartTimeMs(unsigned long position_ms)
{
	startTime = position_ms;
}

void SegmentDescriptor::setEndTimeMs(unsigned long position_ms)
{
	endTime = position_ms;
}


unsigned long SegmentDescriptor::getStartTimeMs() const
{
	return startTime;
}

unsigned long SegmentDescriptor::getEndTimeMs() const
{
	return endTime;
}

void SegmentDescriptor::setParity(unsigned int value)
{
	parity = value & 0x0F;
}

unsigned int SegmentDescriptor::getParity() const
{
	return parity;
}

size_t SegmentDescriptor::getSize() const
{
	size_t mysize = 0;

	for (int i=0; i< (int) keyframes.size(); ++i)
		mysize += keyframes[i].buffer.size();

	return mysize;
}

size_t SegmentDescriptor::write(std::ofstream & fout) const
{
	unsigned char tmpbuffer[80];

	if (empty())
		return 0;		// storing an empty shot is useless

	unsigned int flags = 0; // reset flags
	if (startTime < 0xFFFFFF)
		flags |= 0x01;		// shot start time is valid (fits in 24 bits)

	if (endTime < 0xFFFFFF)
		flags |= 0x02;		// shot end time is valid

	size_t mysize = getSize();
	if (mysize < 0xFFFFFF)
		flags |= 0x04;		// shot size is valid


	BitOutputStream writer;
	writer.open(tmpbuffer, 80);
	writer.write(flags, 4);							// write flags (4 bits)
	writer.write(parity, 4);						// write shot parity (4 bits)
	writer.write((unsigned int) startTime, 24);		// write shot time
	writer.write((unsigned int) endTime,   24);		// write frame time

	writer.write((unsigned int) mysize, 24); 		// write total buffer size
	writer.close();									// close bit writer

	fout.write((char*)tmpbuffer, SHOT_HEADER_SIZE);	// write header into a FILE

	if (flags == 0x07)		// write data only if all flags are OK
	{
		for (int i=0; i < (int) keyframes.size(); ++i) {
			fout.write((const char *) keyframes[i].buffer.data(), keyframes[i].buffer.size());	// write descriptor into a FILE
		}
		return SHOT_HEADER_SIZE + mysize;
	}
	else
	{
		cerr << "ERROR; wrong flags writing shot: " << flags << endl;
		return SHOT_HEADER_SIZE;
	}
}

size_t SegmentDescriptor::read(std::ifstream & fin, mpeg7cdvs::CdvsServer * cdvsserver)
{
	unsigned char tmpbuffer[80];
	fin.read((char*)&tmpbuffer, SHOT_HEADER_SIZE);

	if (fin.good())
	{
		BitInputStream reader;
		reader.open(tmpbuffer, 80);

		unsigned int flags = reader.read(4);
		parity = reader.read(4);
		startTime = reader.read(24);
		endTime = reader.read(24);
		unsigned int mysize = reader.read(24);
		reader.close();

		if ((flags & 0x01) == 0)
			startTime = 0xFFFFFF; 	// shot start time is not valid

		if ((flags & 0x02) == 0)
			endTime = 0xFFFFFF; 	// shot end time is not valid

		if ((flags & 0x04) == 0)	// buffer size is not valid
			mysize = 0xFFFFFF;

		if (flags == 0x07)		// read data only if all flags are OK
		{
			Buffer readbuf(mysize);
			fin.read(readbuf.sdata(), mysize);	// read data from file

			unsigned int consumed = 0;

			while (consumed < mysize)
			{
				CdvsDescriptor descr;
				consumed += cdvsserver->decode (descr, readbuf.data() + consumed, readbuf.size() - consumed);
				keyframes.push_back(descr);
			}

			return SHOT_HEADER_SIZE + mysize;
		}

		return SHOT_HEADER_SIZE;
	}

	return 0;
}

size_t SegmentDescriptor::addSegmentToDB(std::ifstream & fin, mpeg7cdvs::CdvsServer * cdvsserver, const std::string & relativepathname)
{
	unsigned char tmpbuffer[80];
	fin.read((char*)&tmpbuffer, SHOT_HEADER_SIZE);

	if (fin.good())
	{
		BitInputStream reader;
		reader.open(tmpbuffer, 80);

		unsigned int flags = reader.read(4);
		int parity = reader.read(4);
		int startTime = reader.read(24);
		int endTime = reader.read(24);
		unsigned int mysize = reader.read(24);
		reader.close();

		if ((flags & 0x01) == 0)
			startTime = 0xFFFFFF; 	// shot start time is not valid

		if ((flags & 0x02) == 0)
			endTime = 0xFFFFFF; 	// shot end time is not valid

		if ((flags & 0x04) == 0)	// buffer size is not valid
			mysize = 0xFFFFFF;

		if (flags == 0x07)		// read data only if all flags are OK
		{
			Buffer readbuf(mysize);
			fin.read(readbuf.sdata(), mysize);	// read data from file

			unsigned int consumed = 0;

			while (consumed < mysize)
			{
				CdvsDescriptor descr;
				consumed += cdvsserver->decode (descr, readbuf.data() + consumed, readbuf.size() - consumed);
				if (descr.featurelist.nFeatures() > 0)
					cdvsserver->addDescriptorToDB(descr, relativepathname.c_str());	// add image to DB
			}

			return SHOT_HEADER_SIZE + mysize;
		}

		return SHOT_HEADER_SIZE;
	}

	return 0;
}


///////////////////////////////// class CdvaImpl //////////////////////////////////


CdvaImpl::CdvaImpl():verboseMode(false),current_op(UNKNOWN),skip_before(4), skip_after(4), drop_frame_th(0.7),cdvsMode(0), shot_ver_th(18),
		shot_cut_th(2.20), max_retrieved(50), calc_desc_sizes(false)
{
	cdvsconfig = NULL;
	cdvsclient = NULL;
	cdvsserver = NULL;
}

CdvaImpl::~CdvaImpl()
{
	delete cdvsclient;
	delete cdvsserver;
	delete cdvsconfig;
}

void CdvaImpl::init(OPERATION op, bool verbose, size_t n_videos, int querybitrate, int refbitrate, bool calcdescsizes)
{
	current_op = op;
	verboseMode = verbose;
	calc_desc_sizes = calcdescsizes;

	cdvsMode = 0;

	cdvsconfig = CdvsConfiguration::cdvsConfigurationFactory();

	if (current_op == EXTRACT)
		cdvsclient = CdvsClient::cdvsClientFactory(cdvsconfig, cdvsMode); // use ALP

	cdvsserver = CdvsServer::cdvsServerFactory(cdvsconfig);		// used in extract, match and retrieve

	shot_cut_th = 1.98;
	shot_ver_th = 18;
	max_retrieved = 50;

	switch (querybitrate) {
	case  0:
	case 16:
		skip_before = 4;
		skip_after = 4;
		drop_frame_th = 0.7;
		break;
	case 64:
		skip_before = 3;
		skip_after = 3;
		drop_frame_th = 0.6;
		break;
	case 256:
		skip_before = 2;
		skip_after = 2;
		drop_frame_th = 0.5;
		break;
	}

	// prepare DB (in case retrieval is performed)
	if (current_op == RETRIEVE)
		cdvsserver->createDB(cdvsMode, n_videos * 100);		// assuming an average value of 100 keyframes per video
}

void CdvaImpl::close()
{
	if (current_op == RETRIEVE)
		cdvsserver->clearDB();
}

void CdvaImpl::commitDB()
{
	cdvsserver->commitDB();
	cout << "Index done: stored " << cdvsserver->sizeofDB() << " keyframes."<< endl;
}

/*
* Check valid bitrates
*/
bool CdvaImpl::checkBitrate(int bitrate)
{
	switch (bitrate)
	{
	case   0: return true;
	case  16: return true;
	case  64: return true;
	case 256: return true;
	}
	return false;
}

inline int skipFrames(int framenum, VideoCapture & vc)
{
	int skipped = 0;
	while ((skipped < framenum) && vc.grab())
		skipped++;

	return skipped;
}

inline double getCurrentTimeMillis(VideoCapture & vc)
{
	return ((int) vc.get(CV_CAP_PROP_FOURCC) == FOURCC_MP43)? 100 * vc.get(CV_CAP_PROP_POS_MSEC) : vc.get(CV_CAP_PROP_POS_MSEC);	// fix codec-specific issues
}

inline double getCurrentTimeSeconds(VideoCapture & vc)
{
	return ((int) vc.get(CV_CAP_PROP_FOURCC) == FOURCC_MP43)? 0.1 * vc.get(CV_CAP_PROP_POS_MSEC) : 0.001 * vc.get(CV_CAP_PROP_POS_MSEC);	// fix codec-specific issues
}


/*
* CDVA descriptor extension; may be changed (if needed) to identify a specific implementation.
*/
const char * CdvaImpl::getDescriptorExt(int bitrate)
{
	switch (bitrate)
	{
	case   0: return ".DB.cdva";
	case  16: return ".16K.cdva";
	case  64: return ".64K.cdva";
	case 256: return ".256K.cdva";
	}
	return NULL;
}

//
// return the file extension (in lowercase format)
//
string CdvaImpl::getExt(const string & imageName)
{
	size_t found = imageName.find_last_of('.');
	string s = imageName.substr(found + 1, imageName.length());
	transform(s.begin(),s.end(),s.begin(),::tolower);
	return s;
}


// update the bit counters
void CdvaImpl::updateCounters(const CdvsDescriptor & desc, int total_bit_count, ExtractData & outdata) const
{
	if (calc_desc_sizes)
	{
		static const int hc = 64;	// header bit count - the CDVS header size is 3 + 8 + 1 + 1 +1 +2 (align) + 16 + 16 + 16 = 16x4 =  64 bits (fixed)

		int gc = desc.scfvSignature.compressedNumBits();
		int lc = 6;
		for(std::vector<Feature>::const_iterator f=desc.featurelist.features.begin(); f<desc.featurelist.features.end(); ++f)
		{
			for(unsigned int i=0; i< desc.featurelist.qdescr_size; i++)
			{
					if(f->qdescr[i] == 1)
					{
						lc += 1;
					}
					else
					{
						lc += 2;
					}
			}
		}

		if (cdvsconfig->getParameters(desc.getModeID()).numRelevantPoints > 0)		// relevance bits are present --> update lc
		{
			lc += desc.featurelist.nFeatures();
		}

		int cc = (total_bit_count - (hc + gc + lc));		// the remaining bits belong to coordinate coding

		// cout << hc << " " << gc << " " << lc << " " << cc << " " << endl;

		// update counters

		outdata.header_bit_count += hc;
		outdata.global_bit_count += gc;
		outdata.local_bit_count += lc;
		outdata.coordinate_bit_count += cc;
	}

	++ outdata.n_keyframes;				// update keyframe counter
}



/*
 * Check if this is a key frame (local function).
 */
static void computeColorHistogram(const Mat & src, Mat & r_hist, Mat & g_hist, Mat & b_hist)
{
	if( src.empty() )
		  return;

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split( src, bgr_planes );

	/// Establish the number of bins
	int histSize = 32;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	/// Compute the histograms:
	calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist);
	normalize(g_hist, g_hist);
	normalize(r_hist, r_hist);
}

static double diffColorHistogram(const Mat & r, const Mat & g, const Mat & b, const Mat & r_other, const Mat & g_other, const Mat & b_other)
{
	if (r_other.empty() || g_other.empty() || b_other.empty())
	{
		return 1000.0;
	}
	else
	{
		/// compute distance from previous keyframe

		double dist1 = norm(r, r_other, NORM_L2);
		double dist2 = norm(g, g_other, NORM_L2);
		double dist3 = norm(b, b_other, NORM_L2);

		return (dist1 + dist2 + dist3);
	}
}


/*
//  display source and keyframe images + their histograms
static void displayImageAndHistogram(const Mat & src, bool is_keyframe, int histSize, const Mat & r_hist, const Mat & g_hist, const Mat & b_hist)
{
	int hist_w = 512; int hist_h = 400;

	string srcwindow = "Source image";
	string keywindow = "Keyframe image";
	string srchistwindow = "RGB Image histogram";
	string keyhistwindow = "RGB Keyframe histogram";

	namedWindow(srcwindow, WINDOW_AUTOSIZE );			// Create a window for display.
	namedWindow(keywindow, WINDOW_AUTOSIZE );
	namedWindow(srchistwindow, WINDOW_AUTOSIZE);
	namedWindow(keyhistwindow, WINDOW_AUTOSIZE );

	moveWindow(srchistwindow, 200, 40);
	moveWindow(srcwindow, 240 + hist_w, 40);
	moveWindow(keyhistwindow, 200, 80 + hist_h);
	moveWindow(keywindow, 240 + hist_w, 80 + hist_h);

	// specify fx and fy and let the function compute the destination image size.
	double fx = (double) hist_h / (double) src.rows;
	Mat resized;
	resize(src, resized, Size(), fx, fx, INTER_LINEAR);
	imshow(srcwindow, resized );                 	// Show our image inside it.

	// Draw the new image histograms for B, G and R
	int bin_w = cvRound( (double) hist_w/histSize );
	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	/// Draw for each channel
	for( int i = 1; i < histSize; i++ )
	{
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist_h * b_hist.at<float>(i-1)) ) ,
						 Point( bin_w*(i), hist_h - cvRound(hist_h * b_hist.at<float>(i)) ),
						 Scalar( 255, 0, 0), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist_h * g_hist.at<float>(i-1)) ) ,
						 Point( bin_w*(i), hist_h - cvRound(hist_h * g_hist.at<float>(i)) ),
						 Scalar( 0, 255, 0), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist_h * r_hist.at<float>(i-1)) ) ,
						 Point( bin_w*(i), hist_h - cvRound(hist_h * r_hist.at<float>(i)) ),
						 Scalar( 0, 0, 255), 2, 8, 0  );
	}

	/// Display histogram
	imshow(srchistwindow, histImage);

	if (is_keyframe)
	{
		imshow(keyhistwindow, histImage );				// show histogram
		imshow( keywindow, resized );                 	// Show our image inside it.
	}

	waitKey(50);
}
*/



/*
* CDVA extract implementation
*/
void CdvaImpl::extract(const std::string & descrname, const std::string & videopathname, int mode, ExtractData & outdata) const
{
	// check the filename extension to discriminate images from videos

	string ext = getExt(videopathname);
	bool isVideo = (ext.compare("mp4") == 0||ext.compare("wmv") == 0||ext.compare("flv") == 0||ext.compare("ogv") == 0||ext.compare("mpg") == 0||ext.compare("3gp") == 0
				  ||ext.compare("mov") == 0||ext.compare("mkv") == 0||ext.compare("avi") == 0||ext.compare("m4v") == 0||ext.compare("mpeg") == 0||ext.compare("ogg") == 0);
	bool isImage = (ext.compare("jpg") == 0||ext.compare("jpeg") == 0||ext.compare("bmp") == 0||ext.compare("png") == 0||ext.compare("tiff") == 0||ext.compare("tif") == 0);

	if (!isImage && !isVideo)
	{
		throw CdvaException(string("Unknown file extension: ").append(ext));
	}

	std::ofstream fout;
	fout.open(descrname.c_str(), std::ofstream::binary);
	if (! fout.is_open())
	{
		throw CdvaException(string("Cannot write: ").append(descrname));
	}

	if (isImage)
	{
		Mat greyimage = imread(videopathname, CV_LOAD_IMAGE_GRAYSCALE);
		SegmentDescriptor output;
		output.keyframes.resize(1);
		unsigned int descsize = cdvsclient->encode(output.keyframes[0], greyimage.cols, greyimage.rows, greyimage.data);
		output.setStartTimeMs(0);
		output.setEndTimeMs(1000);
		output.setParity(1);
		output.write(fout);

		outdata.setNumFrames(1);					// 1 picture
		outdata.setNumShots(1);						// 1 shot
		outdata.setVideoDuration(1.0);				// 1 second (conventional value for a picture)

		updateCounters(output.keyframes[0], 8*descsize, outdata);	// update all counters (including the keyframe counter)

		if (verboseMode)
		{
			cout << videopathname << " compressed into 1 keyframe, 1 shot" << endl;
		}

	}
	else if (isVideo)
	{
		VideoCapture videocpp(videopathname);

		if (!videocpp.isOpened())
		{
			throw CdvaException(string("File not found: ").append(videopathname));
		}

		double tot_frames = 0;		// do not trust the video header information: compute the number of frames that can be read

		// MAIN LOOP: encode all frames using CDVA

		Mat colorimage;				// input image
		Mat greyimage;				// converted greyscale image

		Mat r_hist, g_hist, b_hist;	// current histograms (RGB)
		Mat r_last, g_last, b_last;	// previous histograms (RGB)
		Mat r_shot, g_shot, b_shot;	// shot histograms (RGB)

		int c1 = 0;
		int c2 = 0;
		int current_frame = 0;
		unsigned int shotCounter = 1;	// shot counter
		unsigned long currentTimeMs = 0;	// shot start time in milliseconds
		bool isFirst = true;

		SegmentDescriptor shot;					// create the shot container
		shot.setStartTimeMs(currentTimeMs);		// initialize first shot
		shot.setParity(shotCounter);			// initialize first shot
		shot.setEndTimeMs(currentTimeMs);		// will be modified later

		CdvsDescriptor prevDesc;

		do
		{
			bool showImage = false;

			currentTimeMs = ((unsigned long) getCurrentTimeMillis(videocpp));

			tot_frames += (c1 = skipFrames(skip_before, videocpp));		// skip n frames
			if (!videocpp.read(colorimage)) break;		// read one frame
			current_frame = ++tot_frames; 			// must be done before setting c2!
			tot_frames += (c2 = skipFrames(skip_after, videocpp));		// skip n frames

			computeColorHistogram(colorimage, r_hist, g_hist, b_hist);

			if (isFirst)	// encode fist frame
			{
				r_hist.copyTo(r_last);	// last keyframe == this frame
				g_hist.copyTo(g_last);
				b_hist.copyTo(b_last);

				r_hist.copyTo(r_shot);	// last frame in the shot == this frame
				g_hist.copyTo(g_shot);
				b_hist.copyTo(b_shot);

				CdvsDescriptor desc;
				cvtColor(colorimage, greyimage, cv::COLOR_BGR2GRAY);		// convert to luminance only
				int descsize = cdvsclient->encode(desc, greyimage.cols, greyimage.rows, greyimage.data);
				updateCounters(desc, 8*descsize, outdata);	// update all counters (including the keyframe counter)

				prevDesc = desc;

				shot.keyframes.push_back(desc);			// store the encoded descriptor
				isFirst = false;
				showImage = true;
			}
			// end encoding fist frame
			else	// encode all other frames
			{

				// check if this is a new keyframe
				double distance = diffColorHistogram(r_hist, g_hist, b_hist, r_last, g_last, b_last);
				if (distance > drop_frame_th)
				{
					// check if the new keyframe belongs to the same shot
					double distshot = diffColorHistogram(r_hist, g_hist, b_hist, r_shot, g_shot, b_shot);

					CdvsDescriptor desc;
					cvtColor(colorimage, greyimage, cv::COLOR_BGR2GRAY);		// convert to luminance only
					int descsize = cdvsclient->encode(desc, greyimage.cols, greyimage.rows, greyimage.data);
					updateCounters(desc, 8 * descsize, outdata);	// update all counters (including the keyframe counter)

					if (distshot> shot_cut_th)
					{
						// verify segment boundary using global descriptor
						PointPairs pp = cdvsserver->match(desc, prevDesc, NULL, NULL, MATCH_TYPE_GLOBAL);
						if (pp.global_score <= shot_ver_th) {

							r_hist.copyTo(r_shot);
							g_hist.copyTo(g_shot);
							b_hist.copyTo(b_shot);

							// save previous shot container
							shot.setEndTimeMs(currentTimeMs);		// this shot ends where the next starts
							shot.write(fout);						// save shot into file

							if (verboseMode)
								cout << "shot: " << shotCounter << ", keyframes: " << shot.keyframes.size() << ", start: " << shot.getStartTimeMs() / 1000.0 << ", stop: " << shot.getEndTimeMs() / 1000.0 << endl;

							// prepare new shot container
							shot.clear();
							shot.setStartTimeMs(currentTimeMs);
							shot.setParity(++shotCounter);

							showImage = true;
						}
					}

					r_hist.copyTo(r_last);
					g_hist.copyTo(g_last);
					b_hist.copyTo(b_last);


					shot.keyframes.push_back(desc);			// store the encoded descriptor
				
					prevDesc = desc;
				}
			}	// end encoding all other frames

			// displayImageAndHistogram(colorimage, showImage, 32, r_hist, g_hist, b_hist);

		}	while ((c1 == skip_before) && (c2 == skip_after));		// stop if video does not contain other frames

		// save last shot container
		shot.setEndTimeMs((unsigned long) getCurrentTimeMillis(videocpp));
		shot.write(fout);

		if (verboseMode)
			cout << "shot: "<< shotCounter << ", keyframes: " << shot.keyframes.size() << ", start: " << shot.getStartTimeMs()/1000.0 << ", stop: " << shot.getEndTimeMs()/1000.0 << endl;

		double time_s = getCurrentTimeSeconds(videocpp);	// normalized time in seconds
		outdata.setVideoDuration(time_s);
		outdata.setNumFrames(tot_frames);
		outdata.setNumShots(shotCounter);
	}	// end else

	outdata.setDescriptorLength((double) fout.tellp());		// real descriptor size (in bytes)
	fout.close();
}

/*
* CDVA match implementation:
*/
double CdvaImpl::match(MatchData & matchResults, const std::string & qdescrname, const std::string & rdescrname, int qmode, int rmode)
{
	if (verboseMode) {
		cout << "matching " << qdescrname << " against " << rdescrname << endl;
	}

	// pairwise matching of frame descriptors
	ShotDescriptorList qDescList, rDescList;

	qDescList.reserve(100);
	rDescList.reserve(100);

	parse(qdescrname, qDescList);
	parse(rdescrname, rDescList);

	return match(matchResults, qDescList, rDescList);
}


double CdvaImpl::match(MatchData & matchResults, const ShotDescriptorList& qDescList, const ShotDescriptorList& rDescList)
{
	double maxScore = 0;

	// loop on all shots

	for (int q = 0; q < (int) qDescList.size(); ++q) {
		for (int r = 0; r < (int) rDescList.size(); ++r) {
			for (int i = 0; i < qDescList[q].keyframes.size(); i ++) {
				for (int j = 0; j < rDescList[r].keyframes.size(); j ++) {

					PointPairs ppglobal = cdvsserver->match(qDescList[q].keyframes[i], rDescList[r].keyframes[j], NULL, NULL, MATCH_TYPE_GLOBAL);	// match using Global Descriptor

					if (ppglobal.global_score > ppglobal.global_threshold)
					{
						PointPairs pplocal = cdvsserver->match(qDescList[q].keyframes[i], rDescList[r].keyframes[j], NULL, NULL, MATCH_TYPE_LOCAL); // match using Local Descriptors
						if (pplocal.local_score > pplocal.local_threshold)
						{
							// there is a match
							double score = ppglobal.global_score * pplocal.local_score;

							if (score > maxScore)
								maxScore = score;

							matchResults.setMatchingTime(0.001 * qDescList[q].getStartTimeMs());	// set the query temporal localization (start time)
							matchResults.setMatchingTime(0.001 * qDescList[q].getEndTimeMs());		// set the query temporal localization (end time)
						}
					}
				}
			}
		}
	}

	matchResults.setMatchingScore(maxScore);

	return maxScore;
}


/*
* CDVA makeindex implementation: can be changed freely;
* you can signal errors at any point of your code using "throw CdvaException(string);"
* the following default implementation of makeindex just appends id + descriptor data to the index file.
*/
void  CdvaImpl::makeindex(const std::string & cdva_descriptor, const std::string & relativepathname)
{
	ifstream reader(cdva_descriptor.c_str(), std::ifstream::binary);

	if (!reader.good())
		throw CdvaException(string("Cannot read descriptor: ").append(cdva_descriptor));

	while (SegmentDescriptor::addSegmentToDB(reader, cdvsserver, relativepathname) > 0)
	{}

	reader.close();
}

//
// order MatchData elements by descending score
//
bool CdvaImpl::byDescendingScore(const MatchData & m1, const MatchData & m2) {
	return (m1.getScore() > m2.getScore());
}


/*
* CDVA retrieve implementation: can be changed freely;
* you can signal errors at any point of your code using "throw CdvaException(string);"
*/
void CdvaImpl::retrieve(vector<MatchData> & retrievalResults, const std::string & qdescrname, int qmode)
{
	ShotDescriptorList qDescList;

	parse(qdescrname, qDescList);

	retrievalResults.clear();

	map<string, MatchData> matches; // matches per video

	for (int i = 0; i < qDescList.size(); i++) {
		for (int j = 0; j < qDescList[i].keyframes.size(); j++) {
			vector<RetrievalData> cdvsResults;
			int n = cdvsserver->retrieve(cdvsResults, qDescList[i].keyframes[j], 100);

			// process results (this version saves the top match even if fScore == 0 to avoid empty results)
			for (unsigned int k = 0; k<n; ++k)
			{
				if ((k == 0) || (cdvsResults[k].fScore > 0)) {

					string imgname = cdvsserver->getImageId(cdvsResults[k].index);
					map<string, MatchData>::iterator iter = matches.find(imgname);
					if (iter == matches.end()) {
						MatchData md;
						md.setReferenceID(imgname);
						md.setMatchingScore(cdvsResults[k].fScore);
						md.setMatchingTime(0.001 * qDescList[i].getStartTimeMs());		// (optional) matching time is always related to the query (the reference is known)
						md.setMatchingTime(0.001 * qDescList[i].getEndTimeMs());		// (optional)
						matches[imgname] = md;
					}
					else {
						if (cdvsResults[k].fScore > iter->second.getScore())
						{
							iter->second.setMatchingScore(cdvsResults[k].fScore);
							iter->second.setMatchingTime(0.001 * qDescList[i].getStartTimeMs());	// optional
							iter->second.setMatchingTime(0.001 * qDescList[i].getEndTimeMs());		// optional
						}
					}
				}
			}
		}
	}

	// copy remaining match data entries
	for (map<string, MatchData>::iterator it = matches.begin(); it != matches.end(); ++it) {
		retrievalResults.push_back(it->second);
	}

	// check the number of retrieved images - cannot be more than max_retrieved

	if (retrievalResults.size() > max_retrieved)
	{
		// partially sort results by descending score
		partial_sort(retrievalResults.begin(), retrievalResults.begin() + max_retrieved, retrievalResults.end(), byDescendingScore);
		retrievalResults.resize(max_retrieved);		// limit results to the top 50
	}
	else	// size is less or equal than the threshold
	{
		// sort all results by descending score
		sort(retrievalResults.begin(), retrievalResults.end(), byDescendingScore);
	}


}


void CdvaImpl::parse(const std::string & descFile, ShotDescriptorList& shotList) {

	ifstream reader(descFile.c_str(), std::ifstream::binary);

	if (!reader.good())
		throw CdvaException(string("Cannot read descriptor: ").append(descFile));

	SegmentDescriptor shot;

	while (shot.read(reader, cdvsserver) > 0) {
		shotList.push_back(shot);
		shot.clear();
	}

	reader.close();

}

