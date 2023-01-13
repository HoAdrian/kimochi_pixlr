#ifndef MODEL_H
#define MODEL_H

#include "frame.h"
#include "QMouseEvent"
#include <QObject>
#include <QColor>
#include <QTimer>
#include <vector>
#include <QFileDialog>
#include <iostream>


/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Reviewers: Yunsu Kwon
 *
 * Model handles the logic of playing animation and drawing the pixels in different frame
 */
class Model : public QObject
{
    Q_OBJECT

public:
    /** Initializes Model */
    explicit Model(QObject *parent = nullptr);


private:
    /**
     * @brief Helper that transforms a coordinate from the screen to the image
     * @param int coordinate - a component of the coordinates (either x or y)
     * @param int screenImageLength - The side length of the image displayed on the screen
     */
    int coordinatesScreenToFrames(int coordinate, int screenImageLength);

    /** List of Frame pointers */
    std::vector<Frame*> frames_;

    /** Helper method that serializes all the frames into the .ssp format */
    QByteArray jsonSerialize();

    /** JSON Document */
    QJsonDocument file_;

    /** The frame currently displayed by the animation */
    int currentFrameIndexAnimation_ = 0;

    /** Default selected frames is 0 */
    int currentlySelectedFrame_ = 0;

    /** The defaut fps value */
    int fps_ = 1;



signals:
    /** Signal to sends the image to be animated */
    void animateFrameSignal(QImage pixels);

    /** Signal to display the input image */
    void displayCurrentFrameSignal(QImage pixels);

    /** Signal to send the byte array of the json representation of the frames */
    void displayCurrentPreviewFrameSignal(QImage pixels);

    /** Signal to send the byte array of the json representation of the frames */
    void serializationResultSignal(QByteArray& sspJson);

    /** Signal that sends the index of the newly added frame */
    void newAddedFrameIndexSignal(int frameIndex);

    /** Signal that sends the index of the removed frame*/
    void removedFrameIndexSignal(int frameIndex);


public slots:
    /**
     * @brief Draws the specified pixel of the specified frame and notifies the View to update the display
     * @param int x - x-coordinate on the screen
     * @param int y - y-coordinate on the screen
     * @param QColor color - the color to fill in
     * @param int frameIndex - specifies which frame to draw
     * @param int screenImageLength - The side length of the image displayed on the screen
    */
    void drawPixelSlot(int x, int y, QColor color, int frameIndex, int screenImageLength);

    /**
     * @brief  Erase the specified pixel of the specified frame and notifies the View to update the display
     * @param int x - x-coordinate on the screen
     * @param int y - y-coordinate on the screen
     * @param QColor color - the color to fill in
     * @param int frameIndex - specifies which frame to draw
     * @param int screenImageLength - The side length of the image displayed on the screen
     */
    void erasePixelSlot(int x, int y, QColor color, int frameIndex, int screenImageLength);

    /**
     * @brief Remove a specified frame
     * @param int imageSideLength
     * @param int currentFrameIndex
     */
    void removeFrame(int imageSideLength, int currentFrameIndex);

    /**
     * @brief Deserializes the provided file into a Kimochi Pixlr sprite editor instance
     * @param QJsonDocument jsonDoc- file the QJsonDocument to open in the sprite editor
     */
     void jsonDeserialize(QJsonDocument);

    /**
     * @brief Adds a frame with the specified image size to the end of frames_ and notifies the View the FrameIndex of the newly added frame
     * @param int imageSideLength
     */
    void addFrame(int imageSideLength);

    /**
     *  @brief Updates the fps to adjust the speed of the animation
     *  @param int fps
     */
    void updateFps(int fps);

    /** Flips the image and sends it back to the View to display it */
    void transform(int currentFrameIndex_);

    /** Serializes all the frames into json format following .ssp format */
    void jsonSerializeSlot();

    /** Updates the preview */
    void updatePreviewSlot();

};

#endif // MODEL_H
