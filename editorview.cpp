#include "editorview.h"
#include "ui_editorview.h"

/**
 * Group: Kimochi
 * Class: CS3505
 * Date: November 6, 2022
 *
 * View to display the editor and handle user input
 */
EditorView::EditorView(Model& model ,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EditorView)
{
    ui->setupUi(this);

    //drawing pixels
    connect(this,
            &EditorView::changePixelSignal,
            &model,
            &Model::drawPixelSlot);

    //erasing pixels
    connect(this,
            &EditorView::erasingPixelSignal,
            &model,
            &Model::erasePixelSlot);
    connect(this,
            &EditorView::transformPixelSignal,
            &model,
            &Model::transform);

    //DisplayModel
    connect(&model,
            &Model::displayCurrentFrameSignal,
            this,
            &EditorView::displayUpdatedDrawing);

    //DisplayPreview
    connect(&model,
            &Model::displayCurrentPreviewFrameSignal,
            this,
            &EditorView::displayUpdatedDrawingPreview);

    //adding frames
    connect(ui->addFrameButton,
            &QPushButton::clicked,
            this,
            &EditorView::addFrameOnClick);
    connect(this,
            &EditorView::addFrameSignal,
            &model,
            &Model::addFrame);
    connect(&model,
            &Model::newAddedFrameIndexSignal,
            this,
            &EditorView::addFrameAvailable);

    //deleting frames
    connect(this,
            &EditorView::deleteCurrentFrameSignal,
            &model,
            &Model::removeFrame);

    connect(&model,
            &Model::removedFrameIndexSignal,
            this,
            &EditorView::removeFrameAvailable);

    //render the current selected frame
    connect(ui->frameSelectionsBox,
            &QComboBox::currentIndexChanged,
            this,
            &EditorView::frameSelection);

    // Save
    //model produces the file, user saves the frames
    connect(ui->actionSave,
            &QAction::triggered,
            &model,
            &Model::jsonSerializeSlot);

    //model serializes the file and send signal back to view
    connect(&model,
            &Model::serializationResultSignal,
            this,
            &EditorView::save);

    //sending json file to deserialize to the model
    connect(this,
            &EditorView::fileSignal,
            &model,
            &Model::jsonDeserialize);

    // Open
    //model deserialize the file, show frames
    connect(ui->actionOpen,
            &QAction::triggered,
            this,
            &EditorView::open);

    //user moves the fps slider, view update the slider and send siganl to model
    connect(ui->fpsSlider,
            &QSlider::sliderMoved,
            this,
            &EditorView::fpsSliderUpdate);
    connect(this,
            &EditorView::fpsChangeSignal,
            &model,
            &Model::updateFps);

    //If user moves the pixelSize slider, will be call the slot and update the value with view.
    connect(ui->pixelSizeSlider,
            &QSlider::sliderMoved,
            this,
            &EditorView::pixelSizeSliderUpdate);

    //Initialize the pixel size with text view.
    ui->PixelsizeLabel->setText("Pixel Size: " +QString::number(pixelSize_-3));
    ui->pixelSizeSlider->setSliderPosition(0);

    graphic_ = new QGraphicsScene(this);

    ui->drawingCanvas->setScene(graphic_);

    //add the initial frame with the default image size
    emit addFrameSignal(IMAGE_SIDE_LENGTH);
    //render the initial defualt image, image side length on the screen does not matter (except 0) here
    emit changePixelSignal(0,0,QColor(255,255,255,255),currentFrameIndex_,IMAGE_SIDE_LENGTH);

    //Timer that will be used to trigger frame switch for animation re-dr
    timerForAnimation = new QTimer(this);

    //Whenever time interval is up, call updatePreviewSlot
    connect(timerForAnimation,
            &QTimer::timeout,
            &model,
            &Model::updatePreviewSlot);

    //Do not start animation timer until FPS slider is updated
    timerForAnimation->stop();
}


EditorView::~EditorView()
{
    delete ui;
}


void EditorView::displayUpdatedDrawing(QImage image){
    int scaledLength = IMAGE_SIDE_LENGTH * pixelSize_;

    //still double scaled the image as in previous version of displayUpdatedDrawing
    QImage scaledImage = image.scaled(QSize(scaledLength, scaledLength)).scaled(scaledLength,scaledLength);

    //takes the top corner of the scaled image to display on the screen
    QImage topLeft = createSubImage(&scaledImage);

    graphic_->addPixmap(QPixmap::fromImage(topLeft));

    ui->drawingCanvas->setScene(graphic_);
    ui->drawingCanvas->show();
}


void EditorView::displayUpdatedDrawingPreview(QImage image){
    //adjust sacle length for preview window
    int scaledLengthPreview = IMAGE_SIDE_LENGTH * (pixelSize_*0.573);

    //still double scaled the image as in previous version of displayUpdatedDrawing
    QImage scaledImagePreview = image.scaled(QSize(scaledLengthPreview, scaledLengthPreview)).scaled(scaledLengthPreview,scaledLengthPreview);

    //takes the top corner of the scaled image to display on the screen
    QImage topLeftPreview = createSubImagePreview((&scaledImagePreview));

    ui->previewLabel->setPixmap(QPixmap::fromImage(topLeftPreview));
    ui->previewLabel->show();
}


QImage EditorView::createSubImage(QImage* image) {
    QRect rect = QRect(0,0,398,398);
    size_t offset = rect.x() * image->depth() / 8
            + rect.y() * image->bytesPerLine();
    return QImage(image->bits() + offset, rect.width(), rect.height(),
                  image->bytesPerLine(), image->format());
}



QImage EditorView::createSubImagePreview(QImage* image) {
    QRect rect = QRect(0,0,228,228);
    size_t offset = rect.x() * image->depth() / 8
            + rect.y() * image->bytesPerLine();
    return QImage(image->bits() + offset, rect.width(), rect.height(),
                  image->bytesPerLine(), image->format());
}


void EditorView::mouseMoveEvent (QMouseEvent *mouseEvent){
    isSaved = false;
    QRect widgetRect = QRect(0,0,400,400);
    QPoint mousePos = ui->drawingCanvas->mapFromGlobal(QCursor::pos());

    if(widgetRect.contains(mousePos))
    {
        ui->XYcoordinateLabel->setText("X: "+ QString::number(mousePos.x())+" Y:" + QString::number(mousePos.y()));
        int imageScale = IMAGE_SIDE_LENGTH * pixelSize_;

        if(isPenSelected_){
            emit changePixelSignal(mousePos.x(), mousePos.y(), currentPenColor_, currentFrameIndex_, imageScale);
        }else if(isEraserSelected_){
            emit changePixelSignal(mousePos.x(), mousePos.y(), QColor(255,255,255), currentFrameIndex_, imageScale);
        }else if(isMirrorSelected_){
            emit changePixelSignal(ui->drawingCanvas->width()-mousePos.x(), mousePos.y(), currentPenColor_, currentFrameIndex_, imageScale);
            emit changePixelSignal(mousePos.x(), mousePos.y(), currentPenColor_, currentFrameIndex_, imageScale);
        }
    }
}


void EditorView::addFrameOnClick(){
    emit addFrameSignal(IMAGE_SIDE_LENGTH);
}


void EditorView::on_deleteFrameButton_clicked()
{
    emit deleteCurrentFrameSignal(IMAGE_SIDE_LENGTH, currentFrameIndex_);
}


void EditorView::addFrameAvailable(int frameIndex){
    ui->frameSelectionsBox->addItem("       frame" + QString::number(frameIndex));
}

void EditorView::removeFrameAvailable(int frameIndex){
    ui->frameSelectionsBox->removeItem(frameIndex);
    for (int i = frameIndex; i < ui->frameSelectionsBox->count(); i++){
        ui->frameSelectionsBox->setItemText(i, "       frame" + QString::number(i));
    }

    ui->frameSelectionsBox->setCurrentIndex(currentFrameIndex_);
}


void EditorView::frameSelection(int frameIndex){
    currentFrameIndex_ = frameIndex;
    emit changePixelSignal(0, 0, QColor(255,255,255,255), currentFrameIndex_, IMAGE_SIDE_LENGTH);
}


void EditorView::save(QByteArray& sspJson){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"/Desktop/",tr("Sprite (*.ssp)"));
    savedFileName_ = fileName;

    if(fileName.isEmpty()) {
        return;
    } else {
        QFile newFile(fileName);
        if(!newFile.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     newFile.errorString());
            return;
        }
        newFile.write(sspJson);
        newFile.close();
    }
    isSaved = true;
}


void EditorView::open() {
    if (!isSaved){
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "DATA LOSS WARNING", "Files will be lost, would you like to save first?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
            return;
          }
    }
    // Save local file name
    savedFileName_ = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"),
                                                  "/Desktop/", tr("Sprite (*.ssp)"));
    if(savedFileName_.isEmpty()) {
        return;
    }
    else {
        QFile localSpriteFile(savedFileName_);
        if(!localSpriteFile.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     localSpriteFile.errorString());
            return;
        }

        QByteArray byteArray = localSpriteFile.readAll();
        localSpriteFile.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);

        if(parseError.error != QJsonParseError::NoError) {
            return;
        }
        else {
            emit fileSignal(jsonDoc);
        }
    }
}


void EditorView::fpsSliderUpdate(){
    //The slider position to send model for fps
    const int fpsValue = (ui->fpsSlider->sliderPosition()/10);

    if (fpsValue == 0){
        timerForAnimation->stop();
    }
    else {
        //start animation timer
        timerForAnimation->start(1000/fpsValue);
        timerForAnimation->setInterval(1000/fpsValue);
    }

    //The fpsValue will be sent to the model updateFps() slot
    emit fpsChangeSignal(fpsValue);
    ui->fpsLabel->setText("Fps: " +QString::number(fpsValue));
}


void EditorView::pixelSizeSliderUpdate(){
    //pixelSize relative to the slider scale(0-100)
    pixelSize_ = ((ui->pixelSizeSlider->sliderPosition()/5)+1)+3;
    ui->PixelsizeLabel->setText("Pixel Size: " +QString::number(pixelSize_-3));
    int imageScale = IMAGE_SIDE_LENGTH * pixelSize_;
    emit changePixelSignal(0, 0, QColor(255,255,255), currentFrameIndex_, imageScale);
}


void EditorView::on_actionPen_triggered()
{
    isPenSelected_ = true;
    isMirrorSelected_ = false;
    isEraserSelected_ = false;

    ui->actionEraser->setChecked(false);
    ui->actionMirror->setChecked(false);
    ui->actionTransform->setChecked(false);
    ui->actionColor->setChecked(false);
}


void EditorView::on_actionEraser_triggered()
{
    isEraserSelected_ = true;
    isMirrorSelected_ = false;
    isPenSelected_ = false;


    ui->actionPen->setChecked(false);
    ui->actionMirror->setChecked(false);
    ui->actionTransform->setChecked(false);
    ui->actionColor->setChecked(false);
}


void EditorView::on_actionTransform_triggered()
{
    isMirrorSelected_ = false;
    isEraserSelected_ = false;
    isPenSelected_ = false;
    emit transformPixelSignal(currentFrameIndex_);

    ui->actionEraser->setChecked(false);
    ui->actionMirror->setChecked(false);
    ui->actionPen->setChecked(false);
    ui->actionColor->setChecked(false);
}


void EditorView::on_actionMirror_triggered()
{
    isMirrorSelected_ = true;
    isEraserSelected_ = false;
    isPenSelected_ = false;


    ui->actionEraser->setChecked(false);
    ui->actionPen->setChecked(false);
    ui->actionTransform->setChecked(false);
    ui->actionColor->setChecked(false);
}


void EditorView::on_actionColor_triggered()
{
    QColor color = QColorDialog::getColor();
    currentPenColor_ = color;


    ui->actionEraser->setChecked(false);
    ui->actionPen->setChecked(false);
    ui->actionTransform->setChecked(false);
    ui->actionMirror->setChecked(false);
}


void EditorView::on_actionBasic_Features_triggered()
{
    QMessageBox::information(
                this,
                tr("Kimochi Pixlr Drawing Features"),
                tr("<b>Draw</b> (f1) - Click the pen icon and then drag mouse on drawing canvas. \n"
                   "<b>Erase</b> (f2) - Click eraser icon and then drag mouse over pixels to erase.\n"
                   "<b>Mirror</b> (f3) - Click icon and then drag mouse on canvas, two mirrored pixels will be drawn. \n"
                   "<b>Transform</b> (f4) - Image will be flipped. Make sure pixel size is small enough to see whole image. \n"
                   "<b>Color Picker</b> (f5) - Click the color palate, choose the color you want, hit 'ok'. \n") );
}


void EditorView::on_actionExtra_Features_triggered()
{
    QMessageBox::information(
                this,
                tr("Kimochi Pixlr"),
                tr("<b>Add Frame</b> - Adds new frame, to access use frame dropdown menu.\n"
                   "<b>Delete Frame</b> - Deletes the current frame, and resets back to the first frame. \n"
                   "<b>Save</b> - Click to open up file saver and save to local computer as an ssp file. \n"
                   "<b>Open</b> - Click to open any .ssp file.") );
}
