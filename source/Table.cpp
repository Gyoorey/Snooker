#include "Table.hpp"

Table::Table(cv::Mat& image):ul(0.0,0.0),ur(0.0,0.0),bl(0.0,0.0),br(0.0,0.0),ml(0.0,0.0),mr(0.0,0.0),ulR(0.0,0.0),urR(0.0,0.0),blR(0.0,0.0),brR(0.0,0.0),mlR(0.0,0.0),mrR(0.0,0.0),H(),mask(){
	segmentRGB(image);
	std::vector<uint16_t> stats = biggestRegion();
	segmentField(image,stats);
}

Table::Table(){
}

bool Table::hasMask(){
	return !(mask.empty());
}

void Table::segmentRGB(cv::Mat& rgbImg){
	mask = cv::Mat::zeros(rgbImg.rows,rgbImg.cols,CV_8UC1);
	for(unsigned i=0;i<rgbImg.rows;i++){
		for(unsigned j=0;j<rgbImg.cols;j++){
			if( (rgbImg.at<cv::Vec3b>(i,j)[1]-rgbImg.at<cv::Vec3b>(i,j)[0]>GREEN_AND_OTHER_DIFF) && (rgbImg.at<cv::Vec3b>(i,j)[1]-rgbImg.at<cv::Vec3b>(i,j)[2]>GREEN_AND_OTHER_DIFF) ){
				mask.at<uint8_t>(i,j) = 1;
			}
		}
	}
	cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );	
	cv::imshow( "Display window", mask*200 ); 
	cv::waitKey(0);
}

std::vector<uint16_t> Table::biggestRegion(){
	cv::Mat labelImage(mask.size(), CV_32S);
	cv::Mat stats(mask.size(), CV_32S);
	cv::Mat centroids(mask.size(), CV_32S);
	int n = cv::connectedComponentsWithStats(mask,labelImage,stats,centroids);
	unsigned int maxArea = 0;
	uint16_t maxIndex = 0;
	for(int i=1;i<n;i++){
		if(maxArea < stats.at<int32_t>(i,cv::CC_STAT_AREA)){
			maxArea = stats.at<int32_t>(i,cv::CC_STAT_AREA);
			maxIndex = i;
		}
	}
	mask = cv::Mat::zeros(mask.rows,mask.cols,CV_8UC1);
	for(int i=0;i<mask.rows;i++){
		for(int j=0;j<mask.cols;j++){
			if(labelImage.at<int32_t>(i,j) == maxIndex){
				mask.at<uint8_t>(i,j) = 1;
			}
		}
	}
	std::vector<uint16_t> ret = std::vector<uint16_t>();
	ret.push_back(stats.at<int32_t>(maxIndex,cv::CC_STAT_LEFT));
	ret.push_back(stats.at<int32_t>(maxIndex,cv::CC_STAT_TOP));
	ret.push_back(stats.at<int32_t>(maxIndex,cv::CC_STAT_WIDTH));
	ret.push_back(stats.at<int32_t>(maxIndex,cv::CC_STAT_HEIGHT));
	return ret;
}

void Table::segmentField(cv::Mat& rgbImg, std::vector<uint16_t> stats){
	//std::vector<cv::Mat> rgbChannels(3);
	//cv::split(rgbImg, rgbChannels);
	int edgeThresh = 15;
	cv::Mat gray, edge;
    cv::cvtColor(rgbImg, gray, cv::COLOR_BGR2GRAY);
    cv::multiply(gray,mask,gray);
	cv::blur(gray, edge, cv::Size(3,3));
	cv::Canny(edge, edge, edgeThresh, edgeThresh*3, 3);
	//calculate points for line fittings
	//middle points
	int m1 = stats[0]+stats[2]/2;
	int m2 = stats[0]+stats[2]/2 - stats[2]/10;
	int m3 = stats[0]+stats[2]/2 + stats[2]/10;
	//two side walls
	int sC1 = round(stats[1]+stats[3]/6);
	int sC2 = round(stats[1]+5*stats[3]/6);
	//get left points
	cv::Point2i lC1,lC2;
	for(int i=0;i<edge.cols;i++){
		if(edge.at<uint8_t>(sC1,i) > 0){
			lC1.x = i;
			lC1.y = sC1;
			break;
		}
	}
	for(int i=0;i<edge.cols;i++){
		if(edge.at<uint8_t>(sC2,i) > 0){
			lC2.x = i;
			lC2.y = sC2;
			break;
		}
	}
	//get right points
	cv::Point2i rC1,rC2;
	for(int i=edge.cols;i>0;i--){
		if(edge.at<uint8_t>(sC1,i) > 0){
			rC1.x = i;
			rC1.y = sC1;
			break;
		}
	}
	for(int i=edge.cols;i>0;i--){
		if(edge.at<uint8_t>(sC2,i) > 0){
			rC2.x = i;
			rC2.y = sC2;
			break;
		}
	}
	//get upper points
	cv::Point2i uM;
	std::vector<uint16_t> upperPoints;
   	bool hasM1=false,hasM2=false,hasM3=false;
   	for(uint16_t i=0;i<edge.rows;i++){
   		if( (rgbImg.at<cv::Vec3b>(i,m1)[1]-rgbImg.at<cv::Vec3b>(i,m1)[0]>GREEN_AND_OTHER_DIFF) && (rgbImg.at<cv::Vec3b>(i,m1)[1]-rgbImg.at<cv::Vec3b>(i,m1)[2]>GREEN_AND_OTHER_DIFF) && !hasM1){
   		//if(!hasM1 && edge.at<uint8_t>(i,m1) > 0){
   			hasM1 = true;
   			upperPoints.push_back(i);
   			std::cout << "1: " << i << std::endl;
   			cv::circle(edge,cv::Point2i(m1,i),5,cv::Scalar(128,128,128),3);
   		}
   		if( (rgbImg.at<cv::Vec3b>(i,m2)[1]-rgbImg.at<cv::Vec3b>(i,m2)[0]>GREEN_AND_OTHER_DIFF) && (rgbImg.at<cv::Vec3b>(i,m2)[1]-rgbImg.at<cv::Vec3b>(i,m2)[2]>GREEN_AND_OTHER_DIFF) && !hasM2){
   		//if(!hasM2 && edge.at<uint8_t>(i,m2) > 0){
   			hasM2 = true;
   			upperPoints.push_back(i);
   			std::cout << "2: " << i << std::endl;
   			cv::circle(edge,cv::Point2i(m2,i),5,cv::Scalar(128,128,128),3);
   		}
   		if( (rgbImg.at<cv::Vec3b>(i,m3)[1]-rgbImg.at<cv::Vec3b>(i,m3)[0]>GREEN_AND_OTHER_DIFF) && (rgbImg.at<cv::Vec3b>(i,m3)[1]-rgbImg.at<cv::Vec3b>(i,m3)[2]>GREEN_AND_OTHER_DIFF) && !hasM3){
   		//if(!hasM3 && edge.at<uint8_t>(i,m3) > 0){
   			hasM3 = true;
   			upperPoints.push_back(i);
   			std::cout << "3: " << i << std::endl;
   			cv::circle(edge,cv::Point2i(m3,i),5,cv::Scalar(128,128,128),3);
   		}
   		if(hasM1 && hasM2 && hasM3){
   			uM.x = m1;
   			uM.y = round( (upperPoints[0] + upperPoints[1] + upperPoints[2])/3 );
   			break;
   		}
   	}
   	//get buttom points
	cv::Point2i bM;
	std::vector<uint16_t> buttomPoints;
   	hasM1=false;
   	hasM2=false;
   	hasM3=false;
   	for(uint16_t i=stats[1]+stats[3];i>0;i--){
   		if(!hasM1 && edge.at<uint8_t>(i,m1) > 0){
   			hasM1 = true;
   			buttomPoints.push_back(i);
   		}
   		if(!hasM2 && edge.at<uint8_t>(i,m2) > 0){
   			hasM2 = true;
   			buttomPoints.push_back(i);
   		}
   		if(!hasM3 && edge.at<uint8_t>(i,m3) > 0){
   			hasM3 = true;
   			buttomPoints.push_back(i);
   		}
   		if(hasM1 && hasM2 && hasM3){
   			bM.x = m1;
   			bM.y = round( (buttomPoints[0] + buttomPoints[1] + buttomPoints[2])/3 );
   			break;
   		}
   	}
   	//visualize points
   	//cv::circle(edge,uM,5,cv::Scalar(128,128,128),3);
   	//cv::circle(edge,bM,5,cv::Scalar(128,128,128),3);
   	//cv::circle(edge,lC1,5,cv::Scalar(128,128,128),3);
   	//cv::circle(edge,lC2,5,cv::Scalar(128,128,128),3);
   	//cv::circle(edge,rC1,5,cv::Scalar(128,128,128),3);
   	//cv::circle(edge,rC2,5,cv::Scalar(128,128,128),3);
   	
   	//calculate corners
   	float lDx = lC1.x - lC2.x;
   	float lDy = lC2.y - lC1.y;
   	float lSlope = lDy/lDx;
   	ul.x = (float)(lC1.x + (lC1.y-uM.y)/lSlope);
   	ul.y = (float)(uM.y);
   	bl.x = (float)(lC2.x - (bM.y-lC2.y)/lSlope);
   	bl.y = (float)(bM.y);
   	float rDx = rC2.x - rC1.x;
   	float rDy = rC2.y - rC1.y;
   	float rSlope = rDy/rDx;
   	ur.x = (float)(rC1.x - (rC1.y-uM.y)/lSlope);
   	ur.y = (float)(uM.y);
   	br.x = (float)(rC2.x + (bM.y-rC2.y)/lSlope);
   	br.y = (float)(bM.y);
   	//visualize points
   	//cv::circle(rgbImg,ul,2,cv::Scalar(128,128,255),2);
   	//cv::circle(rgbImg,bl,2,cv::Scalar(128,128,255),2);
   	//cv::circle(rgbImg,ur,2,cv::Scalar(128,128,255),2);
   	//cv::circle(rgbImg,br,2,cv::Scalar(128,128,255),2);
   	//cv::circle(rgbImg,uM,2,cv::Scalar(128,128,255),2);
   	//cv::circle(rgbImg,bM,2,cv::Scalar(128,128,255),2);
   	//calculate homography
   	std::vector<cv::Point2f> realPositions;
 	std::vector<cv::Point2f> virtualPositions;
 	//coordinates on image
 	realPositions.push_back(ul);
 	realPositions.push_back(ur);
 	realPositions.push_back(br);
 	realPositions.push_back(bl);
 	realPositions.push_back(uM);
 	realPositions.push_back(bM);
 	//virtual coordinates H=2*W
 	virtualPositions.push_back(cv::Point2f(0,0));
 	virtualPositions.push_back(cv::Point2f(R_TABLE_WIDTH,0));
 	virtualPositions.push_back(cv::Point2f(R_TABLE_WIDTH,2*R_TABLE_WIDTH));
 	virtualPositions.push_back(cv::Point2f(0,2*R_TABLE_WIDTH));
 	virtualPositions.push_back(cv::Point2f(R_TABLE_WIDTH/2,0));
 	virtualPositions.push_back(cv::Point2f(R_TABLE_WIDTH/2,2*R_TABLE_WIDTH));
 	//get homography matrix
 	H = cv::findHomography( realPositions, virtualPositions, 0 );
 	//cv::warpPerspective(edge,edge,H,edge.size());
 	//cv::Mat t_src = cv::Mat::zeros(1,1,CV_64FC2);
 	//cv::Mat t_dst = cv::Mat::zeros(1,1,CV_64FC2);
 	//t_src.at<cv::Vec2d>(0,0) = cv::Vec2d(ur.x,ur.y);
 	//cv::transform(t_src,t_dst,H); //ALWAYS DIVIDE BY 3. element!
 	//try to find points on the cushions (we know points on the outer lines)
 	cv::Point2i i_lC1,i_lC2;
 	cv::Point2i i_rC1,i_rC2;
 	cv::Point2i i_uM;
 	cv::Point2i i_bM;
 	for(int i=lC1.x+2;i<edge.cols;i++){
		if(edge.at<uint8_t>(sC1,i) > 0){
			i_lC1.x = i;
			i_lC1.y = sC1;
			break;
		}
	}
 	for(int i=lC2.x+2;i<edge.cols;i++){
		if(edge.at<uint8_t>(sC2,i) > 0){
			i_lC2.x = i;
			i_lC2.y = sC2;
			break;
		}
	}
	for(int i=rC1.x-2;i>0;i--){
		if(edge.at<uint8_t>(sC1,i) > 0){
			i_rC1.x = i;
			i_rC1.y = sC1;
			break;
		}
	}
	for(int i=rC2.x-2;i>0;i--){
		if(edge.at<uint8_t>(sC2,i) > 0){
			i_rC2.x = i;
			i_rC2.y = sC2;
			break;
		}
	}
	std::vector<uint16_t> i_upperPoints;   	
	hasM1=false;
   	hasM2=false;
   	hasM3=false;
   	for(uint16_t i=uM.y+2;i<edge.rows;i++){
   		if(!hasM1 && edge.at<uint8_t>(i,m1) > 0){
   			hasM1 = true;
   			i_upperPoints.push_back(i);
   		}
   		if(!hasM2 && edge.at<uint8_t>(i,m2) > 0){
   			hasM2 = true;
   			i_upperPoints.push_back(i);
   		}
   		if(!hasM3 && edge.at<uint8_t>(i,m3) > 0){
   			hasM3 = true;
   			i_upperPoints.push_back(i);
   		}
   		if(hasM1 && hasM2 && hasM3){
   			i_uM.x = m1;
   			i_uM.y = floor( (i_upperPoints[0] + i_upperPoints[1] + i_upperPoints[2])/3 )-1;
   			break;
   		}
   	}
   	//now calculate transformed uM and i_uM to get the size of buttom cushion
   	cv::Mat uM_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat i_uM_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat uM_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat i_uM_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	uM_src.at<cv::Vec2d>(0,0) = cv::Vec2d(uM.x,uM.y);
   	i_uM_src.at<cv::Vec2d>(0,0) = cv::Vec2d(i_uM.x,i_uM.y);
   	cv::transform(uM_src,uM_dst,H); //ALWAYS DIVIDE BY 3. element!
   	cv::transform(i_uM_src,i_uM_dst,H); 
   	float distanceBetweenLines = (i_uM_dst.at<cv::Vec2d>(0,0)[1]/i_uM_dst.at<cv::Vec2d>(0,0)[2]) - (uM_dst.at<cv::Vec2d>(0,0)[1]/uM_dst.at<cv::Vec2d>(0,0)[2]);
   	// now transform bM and calculate buttom cushion and transform back
   	cv::Mat bM_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat bM_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	bM_src.at<cv::Vec2d>(0,0) = cv::Vec2d(bM.x,bM.y);
   	cv::transform(bM_src,bM_dst,H);
   	//rectified i_bM
   	cv::Mat r_i_bM_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat r_i_bM_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	r_i_bM_src.at<cv::Vec2d>(0,0) = cv::Vec2d(bM_dst.at<cv::Vec2d>(0,0)[0]/bM_dst.at<cv::Vec2d>(0,0)[2],bM_dst.at<cv::Vec2d>(0,0)[1]/bM_dst.at<cv::Vec2d>(0,0)[2]-floor(distanceBetweenLines));
   	//invert H
   	H_inv = cv::Mat(H.clone());
   	cv::invert(H,H_inv);
   	//backproject r_i_bM
   	cv::transform(r_i_bM_src,r_i_bM_dst,H_inv);
   	i_bM.x = ceil(r_i_bM_dst.at<cv::Vec2d>(0,0)[0]/r_i_bM_dst.at<cv::Vec2d>(0,0)[2]);
   	i_bM.y = ceil(r_i_bM_dst.at<cv::Vec2d>(0,0)[1]/r_i_bM_dst.at<cv::Vec2d>(0,0)[2]);
   	//visualize points
   	cv::circle(rgbImg,i_uM,5,cv::Scalar(128,128,128),3);
   	cv::circle(rgbImg,i_bM,5,cv::Scalar(128,128,128),3);
   	cv::circle(rgbImg,i_lC1,5,cv::Scalar(128,128,128),3);
   	cv::circle(rgbImg,i_lC2,5,cv::Scalar(128,128,128),3);
    cv::circle(rgbImg,i_rC1,5,cv::Scalar(128,128,128),3);
   	cv::circle(rgbImg,i_rC2,5,cv::Scalar(128,128,128),3);

   	//recalculate corners
   	lDx = i_lC1.x - i_lC2.x;
   	lDy = i_lC2.y - i_lC1.y;
   	lSlope = lDy/lDx;
   	ul.x = (float)(i_lC1.x + (i_lC1.y-i_uM.y)/lSlope);
   	ul.y = (float)(i_uM.y);
   	bl.x = (float)(i_lC2.x - (i_bM.y-i_lC2.y)/lSlope);
   	bl.y = (float)(i_bM.y);
   	rDx = i_rC2.x - i_rC1.x;
   	rDy = i_rC2.y - i_rC1.y;
   	rSlope = rDy/rDx;
   	ur.x = (float)(i_rC1.x - (i_rC1.y-i_uM.y)/lSlope);
   	ur.y = (float)(i_uM.y);
   	br.x = (float)(i_rC2.x + (i_bM.y-i_rC2.y)/lSlope);
   	br.y = (float)(i_bM.y);
   	//finish and caculate all coordinates, now we have ul,ur,bl,br
   	//get the rectified ones
   	cv::Mat ulR_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat urR_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat blR_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat brR_src = cv::Mat::zeros(1,1,CV_64FC2); 	
   	cv::Mat ulR_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat urR_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat blR_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat brR_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	ulR_src.at<cv::Vec2d>(0,0) = cv::Vec2d(ul.x,ul.y);
   	urR_src.at<cv::Vec2d>(0,0) = cv::Vec2d(ur.x,ur.y);
   	blR_src.at<cv::Vec2d>(0,0) = cv::Vec2d(bl.x,bl.y);
   	brR_src.at<cv::Vec2d>(0,0) = cv::Vec2d(br.x,br.y);
   	cv::transform(ulR_src,ulR_dst,H);
   	cv::transform(urR_src,urR_dst,H);
   	cv::transform(blR_src,blR_dst,H);
   	cv::transform(brR_src,brR_dst,H);
   	ulR.x = round(ulR_dst.at<cv::Vec2d>(0,0)[0]/ulR_dst.at<cv::Vec2d>(0,0)[2]);
   	urR.x = round(urR_dst.at<cv::Vec2d>(0,0)[0]/urR_dst.at<cv::Vec2d>(0,0)[2]);
   	blR.x = round(blR_dst.at<cv::Vec2d>(0,0)[0]/blR_dst.at<cv::Vec2d>(0,0)[2]);
   	brR.x = round(brR_dst.at<cv::Vec2d>(0,0)[0]/brR_dst.at<cv::Vec2d>(0,0)[2]);	
   	ulR.y = round(ulR_dst.at<cv::Vec2d>(0,0)[1]/ulR_dst.at<cv::Vec2d>(0,0)[2]);
   	urR.y = round(urR_dst.at<cv::Vec2d>(0,0)[1]/urR_dst.at<cv::Vec2d>(0,0)[2]);
   	blR.y = round(blR_dst.at<cv::Vec2d>(0,0)[1]/blR_dst.at<cv::Vec2d>(0,0)[2]);
   	brR.y = round(brR_dst.at<cv::Vec2d>(0,0)[1]/brR_dst.at<cv::Vec2d>(0,0)[2]);
   	//now we can calculate the middle pocket positions
   	mlR.x = ulR.x;
   	mlR.y = ulR.y + (blR.y - ulR.y)/2;
   	mrR.x = urR.x;
   	mrR.y = mlR.y;
   	//backproject
   	cv::Mat ml_src = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat mr_src = cv::Mat::zeros(1,1,CV_64FC2); 	
   	cv::Mat ml_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	cv::Mat mr_dst = cv::Mat::zeros(1,1,CV_64FC2);
   	ml_src.at<cv::Vec2d>(0,0) = cv::Vec2d(mlR.x,mlR.y);
   	mr_src.at<cv::Vec2d>(0,0) = cv::Vec2d(mrR.x,mrR.y);
   	cv::transform(ml_src,ml_dst,H_inv);
   	cv::transform(mr_src,mr_dst,H_inv);
   	ml.x = round(ml_dst.at<cv::Vec2d>(0,0)[0]/ml_dst.at<cv::Vec2d>(0,0)[2]);
   	ml.y = round(ml_dst.at<cv::Vec2d>(0,0)[1]/ml_dst.at<cv::Vec2d>(0,0)[2]);
   	mr.x = round(mr_dst.at<cv::Vec2d>(0,0)[0]/mr_dst.at<cv::Vec2d>(0,0)[2]);
   	mr.y = round(mr_dst.at<cv::Vec2d>(0,0)[1]/mr_dst.at<cv::Vec2d>(0,0)[2]);	
   	
   	//convert mask to polygon
   	mask = cv::Mat::zeros(mask.size(),mask.type());
   	std::vector<cv::Point> corners;
   	corners.push_back(ul);
   	corners.push_back(bl);
   	corners.push_back(br);
   	corners.push_back(ur);
   	std::vector<std::vector<cv::Point> > temp;
   	temp.push_back(corners);
   	cv::fillPoly(mask,temp,cv::Scalar(1));
   	cvtColor(mask, mask, CV_GRAY2BGR); 
   	//cv::multiply(gray,mask,gray);
   	cv::circle(rgbImg,ul,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,bl,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,ur,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,br,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,uM,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,bM,2,cv::Scalar(255,128,255),2);
   	cv::circle(rgbImg,ml,2,cv::Scalar(255,128,255),5);
   	cv::circle(rgbImg,mr,2,cv::Scalar(255,128,255),5);
   	//cv::warpPerspective(edge,edge,H,edge.size());
   	//cv::circle(edge,ulR,2,cv::Scalar(255,128,255),2);
   	//cv::circle(edge,blR,2,cv::Scalar(255,128,255),2);
   	//cv::circle(edge,urR,2,cv::Scalar(255,128,255),2);
   	//cv::circle(edge,brR,2,cv::Scalar(255,128,255),2);
   	//cv::circle(edge,mlR,2,cv::Scalar(255,128,255),5);
   	//cv::circle(edge,mrR,2,cv::Scalar(255,128,255),5);
   	
   	//calculate some constants
   	height = bl.y-ul.y;
   	upperWidth = ur.x-ul.x;
   	lowerWidht = br.x-bl.x;
   	upLowDiff = lowerWidht-upperWidth;
   	upLowDiffPerHeigth = upLowDiff/height;
   	
   	cv::imshow( "edges", rgbImg); 
    cv::waitKey(0);
}


cv::Point2i Table::rectify(cv::Point2i p){
	cv::Point2i dst;
	cv::Mat p_src = cv::Mat::zeros(1,1,CV_64FC2);
	cv::Mat p_dst = cv::Mat::zeros(1,1,CV_64FC2);
	p_src.at<cv::Vec2d>(0,0) = cv::Vec2d(p.x,p.y);
	cv::transform(p_src,p_dst,H);
	dst.x = round(p_dst.at<cv::Vec2d>(0,0)[0]/p_dst.at<cv::Vec2d>(0,0)[2]);
	dst.y = round(p_dst.at<cv::Vec2d>(0,0)[1]/p_dst.at<cv::Vec2d>(0,0)[2]);
	return dst;
}

cv::Point2i Table::backProject(cv::Point2i p){
	cv::Point2i dst;
	cv::Mat p_src = cv::Mat::zeros(1,1,CV_64FC2);
	cv::Mat p_dst = cv::Mat::zeros(1,1,CV_64FC2);
	p_src.at<cv::Vec2d>(0,0) = cv::Vec2d(p.x,p.y);
	cv::transform(p_src,p_dst,H_inv);
	dst.x = round(p_dst.at<cv::Vec2d>(0,0)[0]/p_dst.at<cv::Vec2d>(0,0)[2]);
	dst.y = round(p_dst.at<cv::Vec2d>(0,0)[1]/p_dst.at<cv::Vec2d>(0,0)[2]);
	return dst;
}






























/* Hough transform
Mat dst;
    Mat cdst;
    std::vector<cv::Vec2f> lines;
    cv::Canny(image, dst, 50, 200, 3);
    cv::cvtColor(dst, cdst, CV_GRAY2BGR);  
    HoughLines(dst, lines, 1, CV_PI/180, 200, 0, 0 );
    // draw lines
    for( int i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        cv::Point pt1, pt2;
        double a = std::cos(theta), b = std::sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = round(x0 + 1000*(-b));
        pt1.y = round(y0 + 1000*(a));
        pt2.x = round(x0 - 1000*(-b));
        pt2.y = round(y0 - 1000*(a));
        cv::line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
    }
    imshow("source", image);
    imshow("detected lines", cdst);
    cv::waitKey(0);
*/


