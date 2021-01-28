

#include "GCApplication.h"
//��ʼ������ͼ�͸�����
//maskͼGrabCut�����ж�Ӧ�ڶ������������ͼƬ����Щ����ǰ������Щ���ڱ���
//maskͼֻ�ɴ���������ֵ���ֱ�Ϊ��GC_BGD��GC_FGD��GC_PR_BGD��GC_PR_FGD
//mask��ʼ��Ϊ����������ֵΪGC_BGD
void GCApplication::reset()
{
	if( !mask.empty() )
		mask.setTo(Scalar::all(GC_BGD));

	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear();  prFgdPxls.clear();
	iterCount = 0;
}
//initialize image and winName, and create mask image
void GCApplication::setImageAndWinName( const Mat& _image, const string& _winName  )
{
	if( _image.empty() || _winName.empty() )
		return;
	image = &_image;
	winName = &_winName;
	mask.create( image->size(), CV_8UC1);
	reset();
}

//show image
//��� fgdPxls, bgdPxls, prFgdPxls, prBgdPxls�����ǿգ�����ͼƬ����ʾ��ǵĵ�
void GCApplication::showImage() const
{
	if( image->empty() || winName->empty() )
		return;

	Mat res;
	Mat binMask;
	//���ͼ���Ѿ������ã��򿽱�����ͼ��
	//������ʾ�Ѿ����������ͼ��
	if( !isInitialized )
		image->copyTo( res );
	else
	{
		getBinMask( mask, binMask );
		image->copyTo( res, binMask );
	}

	vector<Point>::const_iterator it;
	for( it = bgdPxls.begin(); it != bgdPxls.end(); ++it )
		circle( res, *it, radius, BLUE, thickness );
	for( it = fgdPxls.begin(); it != fgdPxls.end(); ++it )
		circle( res, *it, radius, RED, thickness );
	for( it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it )
		circle( res, *it, radius, LIGHTBLUE, thickness );
	for( it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it )
		circle( res, *it, radius, PINK, thickness );

	if( rectState == IN_PROCESS || rectState == SET )
		rectangle( res, Point( rect.x, rect.y ), Point(rect.x + rect.width, rect.y + rect.height ), GREEN, 2);

	imshow( *winName, res );
}
//ͨ�����α��Mask
void GCApplication::setRectInMask()
{
	assert( !mask.empty() );
	mask.setTo( GC_BGD );
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image->cols-rect.x);
	rect.height = min(rect.height, image->rows-rect.y);
	(mask(rect)).setTo( Scalar(GC_PR_FGD) );
}

//ͨ����껬������ǰ���ͱ���ģ���е����أ��������أ�
void GCApplication::setLblsInMask(  Point p, bool isPr )
{
	vector<Point> *bpxls, *fpxls;
	uchar bvalue, fvalue;
	//���������£����������´���
	if( !isPr )
	{
		bpxls = &bgdPxls;
		fpxls = &fgdPxls;
		bvalue = GC_BGD;
		fvalue = GC_FGD;
	}
	//�����������´���
	else
	{
		bpxls = &prBgdPxls;
		fpxls = &prFgdPxls;
		bvalue = GC_PR_BGD;
		fvalue = GC_PR_FGD;
	}
	//�ж���shift�������»���ctrl�������£��ֱ�ִ�в���
	//if( flags & BGD_KEY )
	//{
	//	bpxls->push_back(p);
		//circle( mask, p, radius, bvalue, thickness );
	//}
	//if( flags & FGD_KEY )
	//{
	//	fpxls->push_back(p);
	//	circle( mask, p, radius, fvalue, thickness );
	//}
}

/*
//�����Ӧ
void GCApplication::mouseClick( int event, int x, int y, int flags, void* )
{
	// TODO add bad args check
	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels
		{
			bool isb = (flags & BGD_KEY) != 0,
				isf = (flags & FGD_KEY) != 0;

			//���rectStateΪNOT_SET����ctrl����shiftû�����£����������´��룬���þ��ο�
			if( rectState == NOT_SET && !isb && !isf )
			{
				rectState = IN_PROCESS;
				rect = Rect( x, y, 1, 1 );
			}
			//���rectStateΪSET������ctrl����shift�����£����������´��룬���GC_BGD(GC_FGD)
			if ( (isb || isf) && rectState == SET )
				lblsState = IN_PROCESS;
		}
		break;
	case CV_EVENT_RBUTTONDOWN: // set GC_PR_BGD(GC_PR_FGD) labels
		{
			//���rectStateΪSET������ctrl����shift������ʱ�����GC_PR_BGD(GC_PR_FGD)
			bool isb = (flags & BGD_KEY) != 0,
				isf = (flags & FGD_KEY) != 0;
			if ( (isb || isf) && rectState == SET )
				prLblsState = IN_PROCESS;
		}
		break;
	case CV_EVENT_LBUTTONUP:
		//���rectStateΪIN_PROCESS����ȷ������߹����������Σ�����ͨ����������Mask
		if( rectState == IN_PROCESS )
		{
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			rectState = SET;
			setRectInMask();
			assert( bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty() );
			showImage();
		}
		//���lblsStateΪIN_PROCESS����ͨ��ԲȦ���Mask
		if( lblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), false);
			lblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_RBUTTONUP:

		//���prLblsStateΪIN_PROCESS����ͨ��ԲȦ���Mask
		if( prLblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), true);
			prLblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_MOUSEMOVE:
		//���rectStateΪIN_PROCESS��������ƶ�ʱ���ɾ���
		if( rectState == IN_PROCESS )
		{
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			assert( bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty() );
			showImage();
		}//���lblsStateΪIN_PROCESS��������ƶ�ʱ��ԲȦ���Mask
		else if( lblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), false);
			showImage();
		}//���prLblsStateΪIN_PROCESS��������ƶ�ʱ��ԲȦ���Mask
		else if( prLblsState == IN_PROCESS )
		{
			setLblsInMask(flags, Point(x,y), true);
			showImage();
		}
		break;
	}
}
*/

//���lblsState����prLblsState������ΪSET����˵��ͼƬ�Ѿ�������Ǵ�ǰ���ͱ�����
//�����Ѿ��������δ����һ���ˣ���ִ��grabCut��GC_INIT_WITH_MASK��ʽ������ִ��
//GC_INIT_WITH_RECT��ʽ�����bgdPxls�ȱ�����ǣ������´α��
int GCApplication::nextIter()
{
	if( isInitialized )
		grabCut( *image, mask, rect, bgdModel, fgdModel, 1);
	else
	{
		if( rectState != SET )
			return iterCount;

		if( lblsState == SET || prLblsState == SET )
			grabCut( *image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK );
		else
			grabCut( *image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT );

		isInitialized = true;
	}
	iterCount++;

	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear(); prFgdPxls.clear();

	return iterCount;
}


//new add
bool GCApplication::SetRect(cv::Rect& rectangle)
{
	rect=rectangle;
	rectState=SET;
	return true;
}

void GCApplication::SetfgdModelPixel(Point p, bool isPr )
{
	vector<Point> *fpxls;
	uchar fvalue;
	//���ܵ��龰ģ��Ԫ��
	if( !isPr )
	{
		fpxls = &fgdPxls;	
		fvalue = GC_FGD;
	}
	//�龰ģ��Ԫ��
	else
	{
		fpxls = &prFgdPxls;
		fvalue = GC_PR_FGD;
	}
	fpxls->push_back(p);
	//circle( mask, p, radius, fvalue, thickness );
	lblsState =SET;  //���ڴ˴���һ������
}