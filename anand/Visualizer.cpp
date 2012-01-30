/*
 * Visualize.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#include "Visualizer.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>

#include <vector>

namespace PredPreyHunterVisualizer
{
    using std::endl;
    using std::vector;

    //--------------------------------------------------------------------
    // OpenGL initialization functions, copied from
    // http://www.opengl.org/About/FAQ/technical/gettingstarted.htm#0030
    //--------------------------------------------------------------------
    static int sAttribList[] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
            None };

    Visualizer::Visualizer(std::map<int, vector<double> > idColorMapping, int width, int height)
    {
        // OpenGL variables
        Display *dpy;
        XVisualInfo *vi;
        XSetWindowAttributes swa;
        Window win;
        GLXContext cx;

        //Switching off display for condor runs - ADITYA
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                LOG(ERROR) << "Error" << endl;
        }  //ADI

        dpy = XOpenDisplay(0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                LOG(ERROR) << "Error" << endl;
        }  //ADI
        if ((vi = glXChooseVisual(dpy, DefaultScreen(dpy), sAttribList)) == NULL) {
            LOG(ERROR) << "ERROR: Can't find suitable visual!\n";
        }
        swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;
        //  win = XCreateSimpleWindow(dpy, RootWindow(dpy,DefaultScreen(dpy)),
        //                      0, 0, 1000, 1000, 1,
        //                      BlackPixel(dpy, DefaultScreen(dpy)),
        //                      WhitePixel(dpy, DefaultScreen(dpy)));
        win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 30, 30, 500, 500, 0, vi->depth,
                CopyFromParent, vi->visual, CWBackPixel | CWBorderPixel | CWColormap, &swa);
        XMapWindow(dpy, win);
        cx = glXCreateContext(dpy, vi, 0, GL_TRUE);
        glXMakeCurrent(dpy, win, cx);

        // clear window
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
        glPointSize(10);
        glFlush();

        this->idColorMapping = idColorMapping;
    }

    void Visualizer::show(const std::vector<AgentInformation>& vAgentInformationPrevious,
            const std::vector<AgentInformation>& vAgentInformationCurrent)
    {
        AgentInformation agentInformationPrevious, agentInformationCurrent;
        for (int i = 0; i < vAgentInformationCurrent.size(); i++) {
            agentInformationPrevious = vAgentInformationPrevious[i];
            agentInformationCurrent = vAgentInformationCurrent[i];

            // erase old point
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex3f(agentInformationPrevious.position.x, agentInformationPrevious.position.y, 0);
            glEnd();

            VLOG(4) << "prev x " << agentInformationPrevious.position.x;
            VLOG(4) << "prev y " << agentInformationPrevious.position.y;

            vector<double> colorMap = idColorMapping[agentInformationCurrent.agentId];
            VLOG(4) << "r,g,b " << colorMap[0] << ", " << colorMap[1] << ", " << colorMap[2] << endl;

            // draw new pred point
            glBegin(GL_POINTS);
            glColor3f(colorMap[0], colorMap[1], colorMap[2]);
            glVertex3f(agentInformationCurrent.position.x, agentInformationCurrent.position.y, 0);
            glEnd();
            glFlush();
        }
        sleep(1);
    }
}
