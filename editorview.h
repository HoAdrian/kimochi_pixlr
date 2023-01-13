#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include "model.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QMouseEvent>
#include <iostream>
#include <QColorDialog>
#include <QByteArray>
#include <QCoreApplication>
#include <QString>
#include <QVariant>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QComboBox>
#include <QShortcut>
#include <QKeySequence>


QT_BEGIN_NAMESPACE
namespace Ui { class EditorView; }
QT_END_NAMESPACE


/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * Reviewers: Mike Phelps, Shing Hei Ho
 *
 * This class represents the view of the kimochi pixlr editor.
 * The user interface for drawing pixels and loading and saving files
 */
class EditorView : public QMainWindow
{
        Q_OBJECT

public:
        /** Constructs the main window and connects signals and slots */
        EditorView(Model& model, QWidget *parent = nullptr);

        /** Default destructor */
        ~EditorView();

private:
        Ui::EditorView *ui;

        /**
         * @brief Creates a subimage of the scaled image that is the upper left hand
         *        corner, and is scaled to the size of the screen, with two pizels cut off on the
         *        far edges so the QGraphicsView doesn't add scroll bars
         * @param QImage* image
         * @ref   https://stackoverflow.com/questions/12681554/dividing-qimage-to-smaller-pieces
         */
        QImage createSubImage(QImage* image);
        /**
         *  @brief Creates the preview for the sub image
         *  @param QImage* image
         */
        QImage createSubImagePreview(QImage* image);

        /** Default size of the image internally */
        const int IMAGE_SIDE_LENGTH = 100;

        /** Scalar for the image e.g., imageSideLengthOnScreen = IMAGE_SIDE_LENGTH * pixelSize_ */
        int pixelSize_ = 4;

        /** Current frame being edited */
        int currentFrameIndex_ = 0;

        /** Variables of tool selections */
        bool isMirrorSelected_ = false;
        bool isEraserSelected_ = false;
        bool isPenSelected_ = false;

        /** Default color for pen */
        QColor currentPenColor_ = Qt::black;

        /** Saved file name*/
        QString savedFileName_;

        /** Graphic scene on the GraphicView*/
        QGraphicsScene *graphic_;

        /** Timer for animation realization */
        QTimer* timerForAnimation;

        /** Is the file saved */
        bool isSaved = false;


signals:
        /** Signal to notify the model to update a pixel in a particular frame */
        void changePixelSignal(int x, int y, QColor color, int frameIndex, int imageScreenWidth);

        /** Signal to notify the model to erase pixels in a particular frame */
        void erasingPixelSignal(int x, int y, QColor color, int frameIndex, int imageScreenWidth);

        /** Signal to notify the model to update FPS */
        void fpsChangeSignal(int fps);

        /** Signal to add a frame with the specified image size */
        void addFrameSignal(int imageSideLength);

        /** Signal to kickstart frame animations in model */
        void startAnimationSignal();

        /** Signal to send JSON file from view to model */
        void fileSignal(QJsonDocument);

        /** Singnal to transform pixels */
        void transformPixelSignal(int currentFrameIndex);

        /** Signal to delete the current signal */
        void deleteCurrentFrameSignal(int imageSideLength, int currentFrameIndex);


public slots:
        /**
         *  @brief Displays the input image on the drawing canvas with suitable size
         *  @param QImage - image to display drawing
         */
        void displayUpdatedDrawing(QImage);

        /**
         *  @brief Displays the preview of animation of all the frames
         *  @param QImage - image to be previewed
         */
        void displayUpdatedDrawingPreview(QImage);

        /**
         *  @brief Draws the pixel at that location when the mouse is pressed
         *  @param QMouseEvent* e - mouse move event
         */
        void mouseMoveEvent (QMouseEvent *e);

        /**
         * @brief Saves the input byte array that represents the JSON file in .ssp format
         * @param QByteArray& sspJson
         * @ref https://doc.qt.io/qt-6/qtwidgets-tutorials-addressbook-part6-example.html
         */
        void save(QByteArray& sspJson);

        /**
         *  @brief Updates the collection of frames available for user to edit
         *  @param int frameIndex - index of frame
         */
        void addFrameAvailable(int frameIndex);

        /**
         *  @brief Removes the specified frame option from the collection of frames for user to choose
         *  @param int frameIndex - index of frame
         */
        void removeFrameAvailable(int frameIndex);

        /**
         *  @brief Updates the currentFrameIndex_ and render the selected frame
         *  @param int frameIndex - index of frame
         */
        void frameSelection(int frameIndex);

        /** Updates the display of the pixelSize value and display pixel in view */
        void pixelSizeSliderUpdate();

        /** Adds a frame when the add frame button is clicked */
        void addFrameOnClick();

        /** Updates the display of the fps value and notifies the model about teh change of FPS */
        void fpsSliderUpdate();

        /** Opens the JSON .ssp file */
        void open();


private slots:
        /** Notifies that the pen tool is selected */
        void on_actionPen_triggered();

        /** Notifies that the eraser tool is selected */
        void on_actionEraser_triggered();

        /** Notifies that the transform tool is selected */
        void on_actionTransform_triggered();

        /** Notifies that the mirror pen tool is selected */
        void on_actionMirror_triggered();

        /** Changes the current selected color according user choice */
        void on_actionColor_triggered();

        /** Trigger delete frame when clicking on the delete frame button*/
        void on_deleteFrameButton_clicked();

        /** Show basic features help menu when clicking on tht basic feature option */
        void on_actionBasic_Features_triggered();

        /** Show extra features help menu when clicking on tht extra feature option */
        void on_actionExtra_Features_triggered();

};

#endif // EDITORVIEW_H
