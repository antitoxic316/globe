#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QOpenGLFunctions_4_5_Core>

#include <cstdlib>

#include <filesystem>

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
private:
    Q_OBJECT

    QOpenGLShaderProgram *globeShaderProgram;
    unsigned int VAO, VBO;
    QOpenGLShaderProgram *markerShaderProgram;
    unsigned int markerVAO, markerVBO;
    QMatrix4x4 projMatrix;

    QString globeFragmentShaderPath;
    QString globeVertexShaderPath;

    QString markerFragmentShaderPath;
    QString markerVertexShaderPath;

    QQuaternion rotation_;
    bool mousePressed_ = false;
    int prevX_ = 0;
    int prevY_ = 0;

    float rotXMultiplier_ = 3.0f;
    float rotYMultiplier_ = 3.0f;

    int minCanvH_ = 300;
    int minCanvW_ = 300;

    int sectorN_ = 10;
    int stackN_ = 10;
    int sphereNumVertices_ = 6*3*sectorN_*stackN_;
    float *sphereVertices_ = (float *)malloc(sphereNumVertices_ * sizeof(float));

    void generateSphereVertices(int sectorN, int stackN);
public:
    Canvas(QWidget *parent = nullptr) : QOpenGLWidget(parent){
        globeFragmentShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/globe.fs");
        globeVertexShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/globe.vs");

        markerFragmentShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/marker.fs");
        markerVertexShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/marker.vs");

        this->setMinimumHeight(minCanvH_);
        this->setMinimumWidth(minCanvW_);
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    };
    ~Canvas(){};
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
};

#endif
