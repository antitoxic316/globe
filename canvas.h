#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLTexture>
#include <QLabel>

#include <cstdlib>
#include <filesystem>

#include "session.h"

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
private:
    Q_OBJECT

    QOpenGLShaderProgram *globeShaderProgram;
    unsigned int VAO, VBO, texVBO, EBO;
    QOpenGLShaderProgram *markerShaderProgram;
    unsigned int markerVAO, markerVBO;
    QMatrix4x4 projMatrix;

    QString globeFragmentShaderPath;
    QString globeVertexShaderPath;

    QString markerFragmentShaderPath;
    QString markerVertexShaderPath;

    QString sphereTexPath_;

    QQuaternion rotation_;
    bool mousePressed_ = false;
    int prevX_ = 0;
    int prevY_ = 0;

    float rotXMultiplier_ = 3.0f;
    float rotYMultiplier_ = 3.0f;

    int sphereR_ = 1;
    int sectorCount = 20;
    int stackCount = 20;

    int sphereNumVertices_ = 3 * (stackCount + 1) * (sectorCount + 1);
    float *sphereVertices_ = (float *)malloc(sphereNumVertices_ * sizeof(float));

    int sphereNumIndeces_ = 6*sectorCount*stackCount;
    unsigned int *sphereIndices_ = (unsigned int *)malloc(sphereNumIndeces_ * sizeof(unsigned int));

    int sphereTexCoordsCount_ = 2 * (stackCount + 1) * (sectorCount + 1);
    float *sphereTexCoords_ = (float *)malloc(sphereTexCoordsCount_ * sizeof(float));
    QOpenGLTexture *globeTex_;

    void generateSphereVertices();

    std::shared_ptr<Session> session_;
private slots:
    void onGlobeRotationUpdated();
public:
    Canvas(QWidget *parent, std::shared_ptr<Session> session)
        : QOpenGLWidget(parent), session_(session)
    {
        globeFragmentShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/globe.fs");
        globeVertexShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/globe.vs");

        markerFragmentShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/marker.fs");
        markerVertexShaderPath.append(std::filesystem::current_path().c_str()).append("/resources/marker.vs");

        sphereTexPath_.append(std::filesystem::current_path().c_str()).append("/resources/1_earth_8k.jpg");

        generateSphereVertices();

        connect(session_.get(), &Session::globeRotated, this, &Canvas::onGlobeRotationUpdated);
    };
    ~Canvas(){
        free(sphereVertices_);
        free(sphereIndices_);
        free(sphereTexCoords_);

        if (globeTex_) delete(globeTex_);
    };
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

    QSize sizeHint() const override { return QSize(400, 400); }
};

#endif
