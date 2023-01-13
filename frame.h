#ifndef FRAME_H
#define FRAME_H

#include <QObject>
#include <iostream>
#include <QImage>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>

/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Reviewers: Siyu Wang, Annabella Miller
 *
 * A frame stores a grid of pixels (image), a name and number representing the frame
 * and the side length of each pixel. An image is assumed to be square.
 */
class Frame : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Initializes the Frame
     * @param int sideLength - side lenghth of internal frame image
     * @param int frameIndex - index of the internal frame image
     * @param QString name - name of the frame
     */
    explicit Frame(int sideLength, int frameIndex, QString name = "frame", QObject *parent = nullptr);

    /**
     * @brief Fills in the pixel at the specified row and column with color
     * @param row to draw the pixel in
     * @param column to draw the pixel in
     * @param color to draw the pixel
     */
    void drawPixel(int row, int column , QColor color);

    /**
     * @brief Erases the pixel at the specified row and column with color
     * @param row to draw the pixel in
     * @param column to draw the pixel in
     * @param color to draw the pixel
     */
    void erasePixel(int row, int column , QColor color);

    /**
     *  @brief Deserializes JSON into a Frame
     *  @param QJsonValue frameSideLength - length of the frame
     *  @param QJsonValue rows - rows of pixels
     */
    void jsonDeserialize(QJsonValue frameSideLength, QJsonValue rows);

    /**
     * @brief Returns the color of the pixel at the specified location
     * @param int row - the row to get the color from
     * @param int column - to get the color from
     */
    QColor getPixelColor(int row, int column);

    /** Serializes the frame into JSON format */
    QJsonArray jsonSerialize();

    /** Returns the image stored in this frame */
    QImage& getImage();

    /** Returns the name of this frame */
    QString getName();

    /** Returns the side length of the square image */
    void setImageSideLength(int length);

    /**
     *  @brief Sets the frame index and the corresponding name
     *  @param int index
     */
    void setFrameIndexAndName(int index);

    /** Returns the side length of the square image */
    int getImageSideLength();

    /** Returns the index*/
    int getFrameIndex();


private:
   /** Side length of the square image */
   int sideLength_;

   /** The index of the frame*/
   int frameIndex_;

   /** The name of the frame*/
   QString name_;


public:
   /** The underlying image of the frame */
    QImage* pixels_;

};

#endif // FRAME_H
