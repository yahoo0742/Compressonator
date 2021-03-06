//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \file acCustomGraphics.cpp
/// \version 2.21
//
//=====================================================================

#include "acCustomGraphics.h"

//=========================================
// Graphics View
//=========================================

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView
// Description: Constructor
// Return Val:   
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsView::acCustomGraphicsView()
{
    ID = 0;
    mousechanged = false;
    //setCacheMode(QGraphicsView::CacheBackground);
    zoomFactor = 1;
}


// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::wheelEvent
// Description: captures mouse wheel events on a view
//              checks if an image is under the mouse
//              and scales it
// Arguments:   QMouseEvent *event
// Return Val:  
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::wheelEvent(QWheelEvent* event)
{
    QGraphicsItem * itemPicked = itemAt(event->pos());
    // Found an item under the cursor
    if (itemPicked)
    {
        // Is the item our custome image 
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::IMAGE))
        {

            acCustomGraphicsImageItem *item = (acCustomGraphicsImageItem *)itemPicked;
            QPointF pos = mapToScene(event->pos());
            QPointF localPt = item->mapFromScene(pos);
            //qDebug() << "Image X" << localPt.rx() << " Y" << localPt.ry();

            //item->setTransformOriginPoint(localPt);

            // If wheel and Shift key move the scale in large chunks
            // else we move in smaller incriments
            //double courseness = (event->modifiers() == Qt::ShiftModifier) ? 2.5 : 0.15;
            //double scaleFactor;
            // Zoom in
            if (event->delta() > 0) {
                emit OnWheelScaleUp(pos);
                // scaleFactor = item->scale() + courseness;
                // if (scaleFactor > 100)  scaleFactor = 100;
                // item->setScale(scaleFactor);
            }
            // Zooming out
            else {
                //scaleFactor = item->scale() - courseness;
                //if (scaleFactor <= 0)    scaleFactor = 0.1;
                //item->setScale(scaleFactor);
                emit OnWheelScaleDown(pos);
            }

            event->accept();

        }
    }

    emit signalWheelEvent(event);
}


// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::mousePressEvent
// Description: captures mouse press events on a view
//              checks if an image is under the mouse
//              and determins if user want to move an item
//              or send a message to reset a view
// Arguments:   QMouseEvent *event
// Return Val:  
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::mousePressEvent(QMouseEvent  *event)
{
    QGraphicsItem * itemPicked = itemAt(event->pos());
    // Found an item under the cursor
    if (itemPicked)
    {
        // Is the item our custome image 
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::IMAGE))
        {
            // User pressed the left mouse change its widget to Hand Cursor
            if (event->button() == Qt::LeftButton) {
                setCursor(Qt::ClosedHandCursor);
                mousechanged = true;
                emit OnMouseHandDown();
            }
        }

        // Item is Navigation and user selected Alt+left mouse Click
        // to resize the Image to fit current view window 
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::NAVIGATION_IMAGE))
        {
            if ((event->button() == Qt::LeftButton) && (event->modifiers() == Qt::ShiftModifier)) {
                emit resetImageView();
            }
        }

    }
    QGraphicsView::mousePressEvent(event);
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::mouseReleaseEvent
// Description: captures mouse release events on a view
//              checks if mouse image has changed if it has it will restore
//              original cursor (Arrow)
// Arguments:   QMouseEvent *event
// Return Val:  
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    // User released the left mouse change restore the cursor widget 
    if (mousechanged)
    {
        setCursor(Qt::ArrowCursor);
        mousechanged = false;
        emit OnMouseHandD();
    }
}



void acCustomGraphicsView::resizeEvent(QResizeEvent *event)
{
    emit ResizeEvent(event);
}




//=========================================
// Custom Scene
//=========================================
static int acCustomGraphicsScene_ID = 0;

acCustomGraphicsScene::acCustomGraphicsScene(QObject *parent = 0) : QGraphicsScene(parent)
{
    acCustomGraphicsScene_ID++;
    ID = acCustomGraphicsScene_ID;

    m_gridStep = 25;
    m_gridenabled = eCustomGraphicsScene_Grids::None;

}

bool acCustomGraphicsScene::isGridEnabled()
{
    return (!(m_gridenabled == eCustomGraphicsScene_Grids::None));
}

void acCustomGraphicsScene::gridEnabled(eCustomGraphicsScene_Grids enable)
{
    m_gridenabled = enable;
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsScene::mousePressEvent
// Description: captures mouse press events on a scene
// Arguments:   QGraphicsSceneMouseEvent *event
// Return Val:  
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsScene::mousePressEvent
// Description: captures mouse press move on a scene
// Arguments:   QGraphicsSceneMouseEvent *event
// Return Val:  
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF scenePos = event->scenePos();

    //qDebug() << "*** acCustomGraphicsScene::mouseMoveEvent **** " << ID << "scenePos: rx:" << scenePos.rx() << "ry: " << scenePos.ry();

    emit sceneMousePosition(&scenePos, ID);

    QGraphicsScene::mouseMoveEvent(event);
}

#include <QPainter> 

void acCustomGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) 
{
    // color Black
    QColor ColorBlack(0, 0, 0);

    // color White
    QColor ColorWhite(255, 255, 255);

    // Draw a line grid
    if (m_gridenabled > 0)
    {
        painter->setPen(Qt::white);

        // Start point for Horizonal steps
        qreal startH = 0;

        //Start point for Virtical steps
        qreal startV = 0;

        switch (m_gridenabled)
        {
        case eCustomGraphicsScene_Grids::Block:
                {
                    QImage image(":/CompressonatorGUI/Images/GridSolid.png");
                    QBrush brush(image);
                    painter->fillRect(rect, brush);
                    break;
                }

        case eCustomGraphicsScene_Grids::Lines:
        {
            // First fill the BackGround as black
            painter->fillRect(rect, ColorBlack);

            // draw horizontal grid lines
            painter->setPen(QPen(ColorWhite));

            for (qreal y = startH; y < rect.bottom();)
            {
                y += m_gridStep;
                painter->drawLine(rect.left(), y, rect.right(), y);
            }

            // draw virtical grid lines   
            for (qreal x = startV; x < rect.right();)
            {
                x += m_gridStep;
                painter->drawLine(x, rect.top(), x, rect.bottom());
            }
            break;
        }

        case eCustomGraphicsScene_Grids::Dots:
        {
            // First fill the BackGround as black
            painter->fillRect(rect, ColorBlack);

            // draw points
            painter->setPen(QPen(ColorWhite));

            for (qreal y = startH; y < rect.bottom();)
            {
                y += m_gridStep;
                // draw virtical grid lines
                for (qreal x = startV; x < rect.right();)
                {
                    x += m_gridStep;
                    painter->drawPoint(x, y);
                }
            }

            break;
        }

        default:
                {
                    painter->fillRect(rect, ColorBlack);
                    break;
                }
        }

    }
    else
        painter->fillRect(rect, ColorBlack);
}

//=========================================
// Custom Graphics Items 
//=========================================

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsImageItem
// Description: Constructor for Image Item
// Return Val:   
// Date:        4/9/2015
// ---------------------------------------------------------------------------

acCustomGraphicsImageItem::acCustomGraphicsImageItem(QPixmap &PixItem) :QGraphicsPixmapItem(PixItem)
{
    ID = 0;
    m_originalPixMap = PixItem;
    setDefaults();
}

void acCustomGraphicsImageItem::setOriginalPixMap(QPixmap &PixItem)
{
    m_originalPixMap = PixItem;
}


void acCustomGraphicsImageItem::setDefaults()
{
    m_alpha = 255;
    
    //====================================================
    // pix = contrast*pix + brightness
    //====================================================
    m_fBrightness = (BRIGHTNESS_INCRIMENTS * 255) / 100;
    m_fContast = 1;

    m_ChannelR = true;
    m_ChannelG = true;
    m_ChannelB = true;
    m_ChannelA = false;
    m_GrayScale = false;
    m_InvertImage = false;
    m_Mirrored = false;
    m_Mirrored_h = true;
    m_Mirrored_v = false;

    m_UseOriginalImage = false;
}

// ---------------------------------------------------------------------------
// Name:   UpdateImage
// Description: Reimplements a PixMap depending on set properties
// Return Val:    none
// Date(DD/MM/YYYY):   02/11/2015
// ---------------------------------------------------------------------------


void acCustomGraphicsImageItem::UpdateImage()
{
    QImage image(m_UseOriginalImage ? m_originalPixMap.toImage() : acCustomGraphicsImageItem::pixmap().toImage());

    // Set channel mapping and Alpha
    for (int x = 0; x < image.width(); x++)
    {
        for (int y = 0; y < image.height(); y++)
        {
            QColor color(image.pixel(x, y));
            if (m_GrayScale)
            {
                // Note qGray() actually computes luminosity using the formula (r*11 + g*16 + b*5)/32.
                int gray = (color.red() + color.green() + color.blue()) / 3;
                image.setPixel(x, y, qRgb(gray, gray, gray));
            }
            else
            {
                if (!m_ChannelR)
                    color.setRed(0);
                if (!m_ChannelG)
                    color.setGreen(0);
                if (!m_ChannelB)
                    color.setBlue(0);
                if (m_ChannelA)
                    color.setAlpha(m_alpha);
                image.setPixel(x, y, color.rgba());
            }
        }
    }
    
    if (m_InvertImage)
    {
        //Changed image.invertPixels(QImage::InvertRgba) to InvertRgb fix the exr invert issue-workaround for now--only exr falls into this if case
        //the InvertRgba seems not working, bmp and png both call else case InvertRgb, that's why they are working fine
        if (image.hasAlphaChannel())
            image.invertPixels(QImage::InvertRgb);
        else
            image.invertPixels(QImage::InvertRgb);
    }

    if (m_Mirrored)
    {
        // Note the flip from what qt defined!
        image = image.mirrored(m_Mirrored_v, m_Mirrored_h);
    }

    if (m_ImageBrightnessUp)
    {
        //---contrast =50
        //if (r>0)
        //    r = ((r - 127) * 50 / 100) + 127;
        //if (g>0)
        //    g = ((g - 127) * 50 / 100) + 127;
        //if (b>0)
        //    b = ((b - 127) * 50 / 100) + 127;
        //if (a>0)
        //    a = ((a - 127) * 50 / 100) + 127;

        //---brightness=50
        //if (r>0)
        //    r= r + 50 * 255 / 100;
        //if (g>0)
        //    g = g + 50 * 255 / 100;
        //if (b>0)
        //    b = b + 50 * 255 / 100;
        //if (a>0)
        //    a = a + 50 * 255 / 100;

        //---gamma= 50 -- not working
        //if (r>0)
        //    r= int(pow(r / 255.0, 100.0 / 50.0) * 255);
        //if (g>0)
        //    g = int(pow(g / 255.0, 100.0 / 50.0) * 255);
        //if (b>0)
        //    b = int(pow(b / 255.0, 100.0 / 50.0) * 255);
        //if (a>0)
        //    a = int(pow(a / 255.0, 100.0 / 50.0) * 255);
        // Set brightness
        
        int r, g, b, a;
        for (int x = 0; x < image.width(); x++)
        {
            for (int y = 0; y < image.height(); y++)
            {
                QColor color(image.pixel(x, y));
                r = color.red();
                g = color.green();
                b = color.blue();
                a = color.alpha();

                if (r > 0)
                {
                    r = (r*m_fContast) + m_fBrightness;
                }

                if (g > 0)
                {
                    g = (g*m_fContast) + m_fBrightness;
                }

                if (b > 0)
                {
                    b = (b*m_fContast) + m_fBrightness;
                }

                if (a > 0)
                {
                    a = (a*m_fContast) + m_fBrightness;
                }
    
                color.setRed(qMin(r,255));
                color.setGreen(qMin(g, 255));
                color.setBlue(qMin(b, 255));
                color.setAlpha(qMin(a, 255));
                image.setPixel(x, y, color.rgba());
            }
        }
    }

    if (m_ImageBrightnessDown)
    {
        int r, g, b, a;
        int pr, pg, pb, pa;
        for (int x = 0; x < image.width(); x++)
        {
            for (int y = 0; y < image.height(); y++)
            {
                QColor color(image.pixel(x, y));
                pr = color.red();
                pg = color.green();
                pb = color.blue();
                pa = color.alpha();
                r = color.red();
                g = color.green();
                b = color.blue();
                a = color.alpha();

                if (pr > 0)
                {
                    r = (r*m_fContast) - m_fBrightness;
                }

                if (pg > 0)
                {
                    g = (g*m_fContast) - m_fBrightness;
                }

                if (pb > 0)
                {
                    b = (b*m_fContast) - m_fBrightness;
                }

                if (pa > 0)
                {
                    a = (a*m_fContast) - m_fBrightness;
                }

                if (r < 0)
                    color.setRed(pr);
                else
                    color.setRed(r);

                if (g < 0)
                    color.setGreen(pg);
                else
                    color.setGreen(g);

                if (b < 0)
                    color.setBlue(pb);
                else
                    color.setBlue(b);

                if (a < 0)
                    color.setAlpha(pa);
                else
                    color.setAlpha(a);
            
                image.setPixel(x, y, color.rgba());
            }
        }
    }


    // Reset the graphics items view
    setPixmap(QPixmap::fromImage(image));
}


// ---------------------------------------------------------------------------
// Name:        changeImage
// Description: Sets the Pixel Maps view with that passed down
// Return Val:   
// Date(M/D):   11/13/2015
// ---------------------------------------------------------------------------

void acCustomGraphicsImageItem::changeImage(QImage image)
{
    setPixmap(QPixmap::fromImage(image));
}



// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsNavImageItem
// Description: Constructor for Navigation Item
// Return Val:   
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsNavImageItem::acCustomGraphicsNavImageItem(QPixmap &PixItem) :QGraphicsPixmapItem(PixItem)
{
}


// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsNavImageItem
// Description: Constructor for  Navigation window item
// Return Val:   
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsNavWindow::acCustomGraphicsNavWindow(QRectF &Rec, QGraphicsItem *parent) :QGraphicsRectItem(Rec, parent)
{
    ID = 0;
}



//
// Central HUB for multiple images to relay mouse positions and events...
//
void acVirtualMouseHub::onVirtualMouseMoveEvent(QPointF *scenePos, QPointF *localPos, int onID)
{
    //qDebug() << "onVirtualMouseHUB " << onID << " rx : " << scenePos->rx() << " ry: " << scenePos->ry();
    emit VirtialMousePosition(scenePos, localPos, onID);
}


void acVirtualMouseHub::onVirtualSignalWheelEvent(QWheelEvent* theEvent, int ID)
{
    Q_UNUSED(theEvent);
    Q_UNUSED(ID);
    //qDebug() << "onVirtualSignalWheelEvent " << ID;
    //emit VirtualSignalWheelEvent(theEvent, ID);
}