#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QMouseEvent>
#include <QDebug>
#include <math.h>

#include "canvas.h"


void Canvas::initializeGL(){
    initializeOpenGLFunctions();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Enable depth testing (for 3D depth calculations)
    glEnable(GL_DEPTH_TEST);
    globeShaderProgram = new QOpenGLShaderProgram(this);

    // Compile shaders and link program
    if (!globeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, globeVertexShaderPath))
        qDebug() << "Vertex shader error:" << globeShaderProgram->log();
    if (!globeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, globeFragmentShaderPath))
        qDebug() << "Fragment shader error:" << globeShaderProgram->log();
    if (!globeShaderProgram->link())
        qDebug() << "Shader link error:" << globeShaderProgram->log();

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    markerShaderProgram = new QOpenGLShaderProgram(this);
    if (!markerShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, markerVertexShaderPath))
        qDebug() << "Vertex shader error:" << markerShaderProgram->log();
    if (!markerShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, markerFragmentShaderPath))
        qDebug() << "Fragment shader error:" << markerShaderProgram->log();
    if (!markerShaderProgram->link())
        qDebug() << "Shader link error:" << markerShaderProgram->log();

    float markerVertices[] = {
        0.0f, 0.0f, 0.125f, // left  
        0.25f, 0.0f, 0.125f, // right
        0.125f,  0.25f, 0.125f  // top 
    }; 

    glGenVertexArrays(1, &markerVAO);
    glGenBuffers(1, &markerVBO);

    glBindVertexArray(markerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, markerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(markerVertices), markerVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
};

void Canvas::resizeGL(int w, int h){
    glViewport(0, 0, w, h); // Set viewport to widget size

    projMatrix.setToIdentity();
    projMatrix.perspective(90.0f, (float)w/h, 0.1f, 100.0f);
};

void Canvas::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    globeShaderProgram->bind();
    globeShaderProgram->setUniformValue("projection", projMatrix);

    QMatrix4x4 model;
    model.translate(0.0f, 0.0f, -2.0f);
    model.rotate(rotation_);
    globeShaderProgram->setUniformValue("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    globeShaderProgram->release();
    
    
    markerShaderProgram->bind();
    markerShaderProgram->setUniformValue("projection", projMatrix);

    qDebug() << rotation_;

    QMatrix4x4 markmodel;
    markmodel.translate(0.0f, 0.0f, -2.0f);
    markerShaderProgram->setUniformValue("model", markmodel);
    markerShaderProgram->setUniformValue("globeAngles", QVector2D(0, 0));
    markerShaderProgram->setUniformValue("rotQuaternion", rotation_.toVector4D());
    markerShaderProgram->setUniformValue("R", 1.0f);


    glBindVertexArray(markerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    markerShaderProgram->setUniformValue("globeAngles", QVector2D(90.0f, 0));
    markerShaderProgram->setUniformValue("rotQuaternion", rotation_.toVector4D());

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    markerShaderProgram->release();
};


void Canvas::mousePressEvent(QMouseEvent *e) {
    mousePressed_ = true;
    prevX_ = e->position().x();
    prevY_ = e->position().y();
}

void Canvas::mouseReleaseEvent(QMouseEvent *e) {
    mousePressed_ = false;
}

void Canvas::mouseMoveEvent(QMouseEvent *e) {
    if(!mousePressed_) return;

    int x = e->position().x();
    int y = e->position().y();

    QVector3D rotEulerAngles = QVector3D(
        ((prevY_ - y) / (double)this->height() * 180.0f/M_PI * rotYMultiplier_),
        ((prevX_ - x) / (double)this->width() * 180.0f/M_PI * rotXMultiplier_),
        0
    );

    rotation_ = QQuaternion::fromEulerAngles(rotEulerAngles) * rotation_;

    prevX_ = x;
    prevY_ = y;

    update();
}


//todo optimize with VBE
static void generateSphereVertices(int sectorN, int stackN){
    float sectorStep = M_PI / sectorN;
    float stackStep = M_PI / stackN;

    float phi = 0;
    float neow = 0;

    for(int i = 0; i < sectorN; i+=6*3){
        sphereVertices_[i] = cos(phi) + cos(neow);
        sphereVertices_[i+1] = sin(phi);
        sphereVertices_[i+2] = cos(phi) + sin(neow);

        phi += stackStep;

        sphereVertices_[i+3] = cos(phi) + cos(neow);
        sphereVertices_[i+4] = sin(phi);
        sphereVertices_[i+5] = cos(phi) + sin(neow);

        phi -= stackStep;
        neow += sectorStep;

        sphereVertices_[i+6] = cos(phi) + cos(neow);
        sphereVertices_[i+7] = sin(phi);
        sphereVertices_[i+8] = cos(phi) + sin(neow);

        sphereVertices_[i+9] = cos(phi) + cos(neow);
        sphereVertices_[i+10] = sin(phi);
        sphereVertices_[i+11] = cos(phi) + sin(neow);

        phi += stackStep;
        neow -= sectorStep;

        sphereVertices_[i+12] = cos(phi) + cos(neow);
        sphereVertices_[i+13] = sin(phi);
        sphereVertices_[i+14] = cos(phi) + sin(neow);

        neow += sectorStep;

        sphereVertices_[i+12] = cos(phi) + cos(neow);
        sphereVertices_[i+13] = sin(phi);
        sphereVertices_[i+14] = cos(phi) + sin(neow);

        phi += stackStep;
    }
}
