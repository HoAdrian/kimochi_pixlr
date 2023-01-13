#include "frame.h"

/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Frame class to represent each frame in the pixel editor
 */
Frame::Frame(int sideLength, int frameIndex, QString name, QObject *parent)
    :QObject{parent}, sideLength_(sideLength), frameIndex_(frameIndex), name_(name)
{
    QImage* img = new QImage(sideLength, sideLength, QImage::Format_ARGB32);
    pixels_ = img;
    name_.append(QString::number(frameIndex));

    //white background
    for (int i = 0; i < pixels_->height(); i++){
        for (int j = 0; j < pixels_->width(); j++){
            pixels_->setPixel(i, j, qRgba(255,255,255,255));
        }
    }
}


void Frame::drawPixel(int row, int column, QColor color){
    pixels_->setPixel(column, row, color.rgba());
}


QImage& Frame::getImage(){
    return *pixels_;
}


QString Frame::getName(){
    return name_;
}


int Frame::getFrameIndex(){
    return frameIndex_;
}


int Frame::getImageSideLength(){
    return sideLength_;
}


QColor Frame::getPixelColor(int row, int column){
    return pixels_->pixel(column, row);
}


void Frame::setFrameIndexAndName(int index) {
    frameIndex_ = index;
    name_ = "frame" + QString::number(index);
}


void Frame::setImageSideLength(int length) {
    sideLength_ = length;
}


QJsonArray Frame::jsonSerialize(){
    QJsonArray frameRows;
    for (int row = 0; row < sideLength_; row++) {
        QJsonArray rowPixels;
        int r, g, b, a;
        for (int col = 0; col < sideLength_; col++) {
            QColor color = pixels_->pixel(col, row);
            color.getRgb(&r, &g, &b, &a);
            QJsonArray rgba = {r, g, b, a};
            rowPixels.append(rgba);
        }
        frameRows.append(rowPixels);
    }
    return frameRows;
}


void Frame::jsonDeserialize(QJsonValue frameSideLength, QJsonValue rows) {
    for (int i = 0; i < frameSideLength.toInt(); i++){
        for (int j = 0; j < frameSideLength.toInt(); j++){
            auto pixels = rows[i].toArray();
            QJsonValue color = pixels.at(j);
            QVariant argbS = color.toVariant();
            QList<QVariant> argb = argbS.toList();
            int r, g, b, a;
            r = argb.at(0).toInt();
            g = argb.at(1).toInt();
            b = argb.at(2).toInt();
            a = argb.at(3).toInt();
            drawPixel(i, j, qRgba(r, g, b, a));
        }
    }
}


void Frame::erasePixel(int row, int column, QColor color){
    pixels_->setPixel(column, row, color.rgba()); //center
    pixels_->setPixel(column + 1, row, color.rgba()); //right
    pixels_->setPixel(column - 1, row, color.rgba()); //left

    pixels_->setPixel(column, row + 1, color.rgba()); //top
    pixels_->setPixel(column , row - 1, color.rgba()); //bottom
    pixels_->setPixel(column + 1, row + 1, color.rgba()); //top-right

    pixels_->setPixel(column - 1, row + 1, color.rgba()); //top-left
    pixels_->setPixel(column - 1, row - 1, color.rgba()); //bottom-left
    pixels_->setPixel(column + 1, row - 1, color.rgba()); //bottom-right
}
