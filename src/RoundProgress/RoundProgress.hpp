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

#ifndef ROUNDPROGRESS_HPP_
#define ROUNDPROGRESS_HPP_

#include <bb/cascades/controls/customcontrol.h>
#include <bb/ImageData>
#include <QtGui/QImage>


namespace bb {
namespace cascades {
class Container;
class ImageView;
class TouchEvent;
}
}

/*!
 *
 * @brief A visual control that is used for displaying Rounded Progress Indicator.
 *
 * This visual control displays only a sector of the image specified by @c value property (clockwise, starting from top center)
 *
 * Here's how to create an @c RoundProgress in QML. In this example, only top right quarter of the image will be displayed:
 *
 * @code
 * import TestRoundProgress 1.0
 *
 * RoundProgress {
 *     horizontalAlignment: HorizontalAlignment.Center
 *     verticalAlignment: VerticalAlignment.Center
 *     value: 90                                                // 1/4 of the image, because by default maxValue=360
 *     imageSource: "asset:///images/outer_progress_bar.png"    // Source of the image. Only local assets are supported now
 * }
 * @endcode
 *
 * The size of the %RoundProgress is calculated according to image size.
 * Manually setting of @c Control::preferredWidth, @c Control::preferredHeight are not tested
 */
class RoundProgress: public bb::cascades::CustomControl {
	Q_OBJECT

	Q_PROPERTY (qreal value READ value WRITE setValue NOTIFY valueChanged)
	Q_PROPERTY (qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged) //=360 by default
	Q_PROPERTY (QString imageSource READ imageSource WRITE setImageSource NOTIFY imageSourceChanged) //should be "asset:///<path_to_image>/<file.png>"

public:
	RoundProgress(bb::cascades::Container *parent = 0);
	virtual ~RoundProgress();

	qreal value() const {return m_value;}
	void setValue(qreal newValue);

	qreal maxValue() const {return m_maxValue;}
	void setMaxValue(qreal newMaxValue);

	QString imageSource() const {return m_imageSource;}
	void setImageSource (const QString& newImageSource);

Q_SIGNALS:
	void valueChanged();
	void maxValueChanged();
	void imageSourceChanged();

private Q_SLOTS:
    void onWidthChanged(float width);
    void onHeightChanged(float height);

private:
    void refresh();
    void onSizeChanged();

    void fillResultImage();
    int getQuarter(qreal value) const;
private:
    qreal m_value;
    qreal m_maxValue;
    QString m_imageSource;

    bb::cascades::Container *m_rootContainer;
    bb::cascades::ImageView *m_imageView;

    float m_width;
    float m_height;

    QImage m_originalImage;
    QImage m_resultImage;

    //Keep previous calculation values to optimize calculation performance
    //Optimization is applied when previous and current values are in the same quarter, and new value is bigger than previous
    bool m_previousStateValid;
    qreal m_previousValue;
    qreal m_tnPrev;
};

#endif /* ROUNDPROGRESS_HPP_ */
