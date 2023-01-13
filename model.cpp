#include "model.h"
#include <iostream>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QTextStream>
#include <QJsonObject>

/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Model handles the logic of playing animation and drawing the pixels in different frames
 */
Model::Model(QObject *parent)
    : QObject{parent}
{
    //empty
}


int Model::coordinatesScreenToFrames(int coordinate, int screenImageLength){
    int numPixels = frames_[0]->getImageSideLength();
    int pixelSize = screenImageLength/numPixels;
    return coordinate/pixelSize;
}


void Model::drawPixelSlot(int x, int y, QColor color, int frameIndex, int screenImageLength){
    currentlySelectedFrame_ = frameIndex;
    int row = coordinatesScreenToFrames(y, screenImageLength);
    int col = coordinatesScreenToFrames(x, screenImageLength);
    frames_[frameIndex]->drawPixel(row, col, color);
    emit displayCurrentFrameSignal(frames_[currentlySelectedFrame_]->getImage());
    emit displayCurrentPreviewFrameSignal(frames_[currentlySelectedFrame_]->getImage());
}


void Model::erasePixelSlot(int x, int y, QColor color, int frameIndex, int screenImageLength){
    currentlySelectedFrame_ = frameIndex;
    int row = coordinatesScreenToFrames(y, screenImageLength);
    int col = coordinatesScreenToFrames(x, screenImageLength);
    frames_[frameIndex]->erasePixel(row, col, color);

    emit displayCurrentFrameSignal(frames_[currentlySelectedFrame_]->getImage());
    emit displayCurrentPreviewFrameSignal(frames_[currentlySelectedFrame_]->getImage());
}


void Model::addFrame(int imageSideLength){
    int newFrameIndex = frames_.size();
    frames_.push_back(new Frame(imageSideLength, newFrameIndex));
    emit newAddedFrameIndexSignal(newFrameIndex);
}


QByteArray Model::jsonSerialize() {

    QJsonObject jsonSSP;
    QJsonObject frames;

    for (Frame* frame : frames_){
        QJsonArray frameRows = frame->jsonSerialize();
        frames[frame->getName()] = frameRows;
    }

    int numFrames = frames_.size();
    int frameHeight = frames_[0]->getImageSideLength();
    int frameWidth = frames_[0]->getImageSideLength();

    jsonSSP["height"] = frameHeight;
    jsonSSP["width"] = frameWidth;
    jsonSSP["numberOfFrames"] = numFrames;
    jsonSSP["frames"] = frames;

    QByteArray byteArray = QJsonDocument(jsonSSP).toJson();
    return byteArray;
}


void Model::updatePreviewSlot(){
    if (fps_ == 0){

        emit displayCurrentPreviewFrameSignal(frames_[currentlySelectedFrame_]->getImage());
    }

    else{
        emit displayCurrentPreviewFrameSignal(frames_[currentFrameIndexAnimation_]->getImage());

        //Make sure we don't try to dispaly a frame index out of bounds.
        //only increment if increment will not set index out of bounds.
        if (currentFrameIndexAnimation_ >= (int)(frames_.size() - 1)){
            currentFrameIndexAnimation_ = 0;
        }
        else {
            emit displayCurrentPreviewFrameSignal(frames_[currentFrameIndexAnimation_]->getImage());
            currentFrameIndexAnimation_ += 1;
        }
    }
}


void Model::jsonSerializeSlot(){
    QByteArray sspJson = jsonSerialize();
    emit serializationResultSignal(sspJson);
}


void Model::jsonDeserialize(QJsonDocument file) {
    //remove all the frames in the program first
    for(unsigned int i=frames_.size()-1; i>=0; i--){
        frames_.pop_back();
        if(i!=0){
            emit removedFrameIndexSignal(i);
        }else{
            break;
        }
    }

    QJsonObject fields = file.object();
    QJsonValue frameHeight = fields.value("height");
    QJsonObject frameN = fields.value("frames").toObject();

    int frameSideLength = frameHeight.toInt();

    // keys represent different frames
    QStringList keys = frameN.keys();
    int keyIndex = 0;
    Frame* newFrame;
    for (auto key : keys) {
        if (keyIndex > 0) {
            addFrame(frameSideLength);
            newFrame = frames_.at(keyIndex);
        }
        else {
            newFrame = new Frame(frameSideLength, 0);
            frames_.push_back(newFrame);
        }
        auto value = frameN.take(key);
        if (value.isArray()) {
            qDebug() << key ;
            // key is the frame name e.g.,"frame0"
            // value is the pixel array (frameRows)
            auto rows = value.toArray();
            newFrame->jsonDeserialize(frameSideLength, rows);
        }
        keyIndex++;
    }

    emit displayCurrentFrameSignal(frames_[0]->getImage());
}


void Model::transform(int currentFrameIndex_){
    currentlySelectedFrame_ = currentFrameIndex_;
    *frames_[currentFrameIndex_]->pixels_ = frames_[currentFrameIndex_]->pixels_->mirrored(true,false);
    emit displayCurrentFrameSignal(frames_[currentlySelectedFrame_]->getImage());
    emit displayCurrentPreviewFrameSignal(frames_[currentlySelectedFrame_]->getImage());
}


void Model::removeFrame(int imageSideLength, int currentFrameIndex)
{
    if (frames_.size() > 1){
        frames_.erase(frames_.begin() + currentFrameIndex);
        for(unsigned int i=currentFrameIndex; i<frames_.size(); i++){
            frames_[i]->setFrameIndexAndName(i);
        }

        emit displayCurrentFrameSignal(frames_[currentFrameIndex]->getImage());
        emit removedFrameIndexSignal(currentFrameIndex);
    }
    else {
        frames_[0] = new Frame(imageSideLength, 0);
        emit displayCurrentFrameSignal(frames_[0]->getImage());
    }
}


void Model::updateFps(int fps){
    if (fps == 0) {
        emit displayCurrentPreviewFrameSignal(frames_[currentlySelectedFrame_]->getImage());
    }
    fps_ = fps;
}
