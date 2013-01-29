/*************************************************************************
Copyright (c) 2013, Michael Nosov (Michael.Nosov@gmail.com)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Neither the name of the "Michael Nosov" nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Michael Nosov ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Michael Nosov BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#include "RoundProgress.hpp"

#include <bb/cascades/AbsoluteLayout>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <qmath.h>
#include <QByteArray>
#include <QFile>
#include <QtGui/QImageReader>
#include <QDebug>

using namespace bb::cascades;
using namespace bb;

RoundProgress::RoundProgress(Container *parent):
	CustomControl(parent),
	m_value(0),
	m_maxValue(360),
	m_previousStateValid(false),
	m_previousValue(0)
{
	m_rootContainer = new Container();
	m_rootContainer->setLayout(new DockLayout());
	m_imageView = ImageView::create().horizontal(HorizontalAlignment::Fill).vertical(VerticalAlignment::Fill);
	m_rootContainer->add(m_imageView);
	setRoot(m_rootContainer);

    connect(this, SIGNAL(preferredHeightChanged(float)), this, SLOT(onHeightChanged(float)));
    connect(this, SIGNAL(preferredWidthChanged(float)), this, SLOT(onWidthChanged(float)));

    m_width = 250;
    m_height = 250;
    setPreferredSize(m_width, m_height);
}

RoundProgress::~RoundProgress() {
	// TODO Auto-generated destructor stub
}

void RoundProgress::setValue(qreal newValue)
{
	m_value = newValue;
	emit valueChanged();
	refresh();
}

void RoundProgress::setMaxValue(qreal newMaxValue)
{
	m_previousStateValid = false;
	m_maxValue = newMaxValue;
	emit maxValueChanged();
	refresh();
}

void RoundProgress::setImageSource (const QString& newImageSource)
{
	m_previousStateValid = false;
	if (!newImageSource.startsWith("asset:///")) {
		qDebug() << "RoundProgress::imageSource: Non-local images are not supported yet";
		return;
	}
	QString localFileSource = newImageSource;
	localFileSource.replace("asset:///", "app/native/assets/");
    QFile file(localFileSource);
    bool resOpen = file.open(QFile::ReadOnly);
    qDebug() << resOpen;
    if (!resOpen) {
        qDebug() << "Failed to open file";
        return;
    }

    QImageReader imgReader(&file);
    QSize s = imgReader.size();
    if (s.width() == 0 || s.height() == 0) {
        qDebug() << "Invalid image size";
        return;
    }
    QImage image;
    if (!imgReader.read(&image)) {
        qDebug() << "Unable to read image";
        return;
    }

    m_originalImage = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

	m_imageSource = newImageSource;
	emit imageSourceChanged();

	if (m_width != s.width() || m_height != s.height()) {
		m_width = s.width();
		m_height = s.height();
		setPreferredSize(m_width, m_height);
	} else {
		refresh();
	}
}

void RoundProgress::onWidthChanged(float width)
{
	m_width = width;
	onSizeChanged();
}

void RoundProgress::onHeightChanged(float height)
{
	m_height = height;
	onSizeChanged();
}

void RoundProgress::refresh()
{
	if (m_imageSource.isEmpty()) {
		// qDebug() << "RoundProgress: No image source yet";
		return;
	}

	fillResultImage(); //changing pixels of m_resultImage

	ImageData pixelBuffer = ImageData::fromPixels(m_resultImage.rgbSwapped().bits(),
								bb::PixelFormat::RGBA_Premultiplied,
								m_width,
								m_height,
								m_resultImage.bytesPerLine());
	Image image = Image(pixelBuffer);
	if (!image.isNull()) {
		m_imageView->setImage(image);
	}
}

int RoundProgress::getQuarter(qreal value) const
{
	if (value <= m_maxValue/4 && value >= 0) {
		return 1;
	} else if (value <= m_maxValue/2) {
		return 2;
	} else if (value <= 3*m_maxValue/4) {
		return 3;
	} else if (value <= m_maxValue) {
		return 4;
	}
	return 0;
}

//Implemented usage of previous value, when previous and current values are in the same quarters
void RoundProgress::fillResultImage() {
	qreal curValue = qBound((qreal)0, m_value, m_maxValue);
	qreal halfWidth = m_width/2;
	int halfHeightInt = m_height/2;
	if (curValue == m_maxValue) {
		m_resultImage = m_originalImage;
		return;
	} else if (curValue == 0) {
		m_resultImage = QImage(QSize(m_width, m_height), QImage::Format_ARGB32_Premultiplied);
		m_resultImage.fill(QColor(0,0,0,0));
		return;
	}
	if (m_previousStateValid && curValue <= m_previousValue) {
		m_previousStateValid = false;
	}
	int curQuarter = getQuarter(curValue);
	int prevQuarter = m_previousStateValid? getQuarter(m_previousValue): 0;
	if (m_previousStateValid && (curQuarter != prevQuarter)) {
		m_previousStateValid = false;
	}
	//qDebug() << "Refreshing:" << curValue << m_previousValue << m_imageSource;
	qreal angle = curValue * M_PI * 2 / m_maxValue;
	qreal tn = 0;
	if (curValue != m_maxValue/4 && curValue != 3*m_maxValue/4) {
		tn = qTan(angle);
	}
	qreal tnPrev = m_previousStateValid? m_tnPrev: 0;
	int yStart, yEnd;
	bool moreThanHalf = curValue > m_maxValue/2;
	if (curQuarter == 1) {
		yStart = 0;
		yEnd = m_height/2;
	} else if (curQuarter == 2) {
		yStart = m_previousStateValid? (m_height/2): 0;
		yEnd = m_height;
	} else if (curQuarter == 3) {
		yStart = m_previousStateValid? (m_height/2): 0;
		yEnd = m_height;
	} else if (curQuarter == 4) {
		yStart = 0;
		yEnd = m_height/2;
	}
	if (!m_previousStateValid) {
		if (!moreThanHalf) {
			m_resultImage = QImage(QSize(m_width, m_height), QImage::Format_ARGB32_Premultiplied);
			m_resultImage.fill(QColor(0,0,0,0));
		} else {
			m_resultImage = m_originalImage;
		}
	}

	int xMin = 0;
	int xMax = 0;
	qreal tmp = 0;
	qreal tmpDelta = -tn;
	if (curValue != m_maxValue/4 && curValue != 3*m_maxValue/4) {
		tmp = (m_height/2 - yStart) * tn;
	} else if (curValue == m_maxValue/4) {
		tmp = m_width/2;
		tmpDelta = 0;
	} else if (curValue == 3*m_maxValue/4) {
		tmp = -m_width/2;
		tmpDelta = 0;
	}

	qreal tmpPrev = 0;
	qreal tmpPrevDelta = m_previousStateValid? -tnPrev: 0;
	if (m_previousStateValid) {
		if (m_previousValue != m_maxValue/4 && m_previousValue != 3*m_maxValue/4) {
			tmpPrev = (m_height/2 - yStart) * tnPrev;
		} else if (m_previousValue == m_maxValue/4) {
			tmpPrev = halfWidth;
			tmpPrevDelta = 0;
		} else if (m_previousValue == 3*m_maxValue/4) {
			tmpPrev = -halfWidth;
			tmpPrevDelta = 0;
		}
	}
	int depth = m_resultImage.depth()/8;
	for (int y = yStart; y < yEnd; y++) {
		xMin = 0;
		xMax = 0;
		if (m_previousStateValid) {
			xMin = qMin(halfWidth+tmpPrev, halfWidth+tmp);
			xMax = qMax(halfWidth+tmpPrev, halfWidth+tmp);
		} else {
			if (curQuarter == 1) {
				xMin = halfWidth;
				xMax = halfWidth+tmp;
			} else if (curQuarter == 2) {
				xMin = (y <= halfHeightInt)? halfWidth: (halfWidth + tmp);
				xMax = m_width;
			} else if (curQuarter == 3) {
				xMin = 0;
				xMax = (y <= halfHeightInt)? halfWidth: (halfWidth + tmp);
			} else if (curQuarter == 4) {
				xMin = halfWidth+tmp;
				xMax = halfWidth;
			}
		}
		xMin = qBound(0, xMin, (int)m_width);
		xMax = qBound(0, xMax, (int)m_width);

		//using memcpy for copy image data. Comment it out if it doesn't work on your images
		if (xMin < xMax) {
			int imgOffset = depth*xMin;
			uchar* dest = m_resultImage.scanLine(y);
			dest += imgOffset;
			if (!moreThanHalf || m_previousStateValid) {
				const uchar* src = m_originalImage.scanLine(y);
				src += imgOffset;
				memcpy(dest, src, (xMax - xMin)*depth);
			} else {
				memset(dest, 0, (xMax - xMin)*depth);
			}
		}

		//Uncomment this is you want to use 'setPixel' for copy image data. This way is up to 2 times slower than memcpy variant
		/*for (int x = xMin; x < xMax; x++) {
			if (!moreThanHalf || m_previousStateValid) {
				m_resultImage.setPixel(x, y, m_originalImage.pixel(x, y));
			} else {
				m_resultImage.setPixel(x, y, 0x00000000);
			}
		}*/
		tmpPrev += tmpPrevDelta;
		tmp += tmpDelta;
	}
	//Save previous values and calculating only delta with previous state. It will increase performance up to ~20%
	m_previousValue = curValue;
	m_previousStateValid = true;
	m_tnPrev = tn;
}

void RoundProgress::onSizeChanged()
{
	m_previousStateValid = false;
	if (!m_imageSource.isEmpty()) {
		refresh();
	}
}

