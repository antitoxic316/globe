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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &texVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    //position
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereNumVertices_*sizeof(float), sphereVertices_, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereNumIndeces_*sizeof(unsigned int), sphereIndices_, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereTexCoordsCount_*sizeof(float), sphereTexCoords_, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    globeTex_ = new QOpenGLTexture(QImage(sphereTexPath_).mirrored(true, false));
    globeTex_->setMinificationFilter(QOpenGLTexture::Nearest);
    globeTex_->setMagnificationFilter(QOpenGLTexture::Linear);
    globeTex_->setWrapMode(QOpenGLTexture::Repeat);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //todo clean up shader program
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
    globeShaderProgram->setUniformValue("texture1", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, globeTex_->textureId());
    glActiveTexture(0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, sphereNumIndeces_, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    globeShaderProgram->release();
    
    markerShaderProgram->bind();
    markerShaderProgram->setUniformValue("projection", projMatrix);


    QList<Marker> markers = session_->getMarkers();

    for(int i = 0; i < markers.size(); i++){
        float lat = markers.at(i).latitude;
        float lon = markers.at(i).longitude;

        QMatrix4x4 markmodel;
        markmodel.translate(0.0f, 0.0f, -2.0f);
        markerShaderProgram->setUniformValue("model", markmodel);
        markerShaderProgram->setUniformValue("globeAngles", QVector2D(lon, lat));
        markerShaderProgram->setUniformValue("rotQuaternion", rotation_.toVector4D());
        markerShaderProgram->setUniformValue("R", 1.0f);

        glBindVertexArray(markerVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

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

    session_->updateGlobeRotation(rotation_.toEulerAngles());

    update();
}


//code is adjusted from https://songho.ca/opengl/gl_sphere.html#example_sphere
void Canvas::generateSphereVertices(){
    float x, y, z, xz;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / sphereR_;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xz = sphereR_ * cosf(stackAngle);             // r * cos(u)
        y = sphereR_ * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            int index = (i * (sectorCount + 1) + j) * 3;

            // vertex position (x, y, z)
            x = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
            z = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
            sphereVertices_[index] = x;
            sphereVertices_[index+1] = y;
            sphereVertices_[index+2] = z;

            int texIndex = (i * (sectorCount+1) + j) * 2;
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            sphereTexCoords_[texIndex] = s;
            sphereTexCoords_[texIndex+1] = t;
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            int offset = 3;

            int index = (i*sectorCount + j) * 6;
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                sphereIndices_[index] = (k1);
                sphereIndices_[index+1] = (k2);
                sphereIndices_[index+2] = (k1 + 1);
            } else {
                offset = 0;
            }


            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                sphereIndices_[index+offset] = (k1 + 1);
                sphereIndices_[index+offset+1] = (k2);
                sphereIndices_[index+offset+2] = (k2 + 1);
            }
        }
    }
}

void Canvas::onGlobeRotationUpdated(){
    rotation_ = QQuaternion::fromEulerAngles(session_->getGlobeEulers());
    update();
}
