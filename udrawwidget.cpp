#include "udrawwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPen>
#include <QMetaObject>
#include <iostream>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <queue>
#include <utility>

using std::vector;
using std::pair;


vector <edge> EMOD(vector <vector2d>& point)
{
    vector <edge> edges;
    int n = point.size();
    for (int i = 0; i < n-1; ++i)
        for (int j = i+1; j < n; ++j)
        {
            edges.push_back( edge(point[i], point[j]));
        }
    int m = int(edges.size());
    vector <pair <double, edge>> g;

    for (int i = 0; i < m; ++i)
    {
        g.push_back(std::make_pair(edges[i].norm(), edges[i]));
    }
    vector <edge> result;
    std::sort(g.begin(), g.end(),[] (const auto &x, const auto &y) { return x.first < y.first; });
    result.push_back(g[0].second);
    vector<pair <int, vector2d>> tree_id(n);
    for (int i = 0; i < n; ++i)
    {
        tree_id[i].first = i;
        tree_id[i].second = point[i];
    }
    for (int i=0; i<m; ++i)
    {
        vector2d a = g[i].second.A, b = g[i].second.B;
        int k_a, k_b;
        for (int j = 0; j < n; ++j)
        {
            if (tree_id[j].second == a) k_a = tree_id[j].first;
            if (tree_id[j].second == b) k_b = tree_id[j].first;
        }
        if (k_a != k_b)
        {
            result.push_back(edge(a, b));
        }
        for (int j=0; j<n; ++j)
                        if (tree_id[j].first == k_a) tree_id[j].first = k_b;
    }

    return result;
}

UDrawWidget::UDrawWidget(QWidget *parent)
    : QWidget{parent}
{
    catched_point = NoPoint;
}


void UDrawWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    for (auto x : this->point)
    {
        p.setPen(Qt::red);
        p.drawEllipse(int(x.x) - 2.5, int(x.y) - 2.5, 5, 5);
    }
    p.setPen(Qt::green);


    if (point.size() > 1) result = EMOD(point);
    if (result.size() > 1)
    {
        for (auto x = result.begin(); x != result.end(); x++)
        {
            auto a = x->A, b = x->B;
            p.drawLine(a.x, a.y, b.x, b.y);
        }
    }

}
void UDrawWidget::mousePressEvent(QMouseEvent* pe)  // методы обработки события мыши при нажатии клавиши мыши
{
    if(pe->button() == Qt::LeftButton)
    {
        bool flag = false;

        if (int(point.size()) == 0)
        {
            this->point.push_back(vector2d(pe->position().x(), pe->position().y()));
        }
        for (int i = 0; i < int(point.size()); i++)
        {
            if (vector2d::dist(point[i], vector2d(pe->position().x(), pe->position().y())) <= 2.5)
            {
                catched_point = Point;
                k=point[i];
                flag = true;
                pr_mouseX = pe->x();
                pr_mouseY = pe->y();
            }
        }
        if (!flag)
        {
            this->point.push_back(vector2d(pe->position().x(), pe->position().y()));
        }
    }
    if(pe->button() == Qt::RightButton)
    {
        auto buf = remove_if(this->point.begin(), this->point.end(), [&](vector2d a)
        {return vector2d::dist(a, vector2d(pe->position().x(), pe->position().y())) <= 2.5;});
        this->point.erase(buf, this->point.end());
    }
     update();

}

void UDrawWidget::Clear()  // очищение экрана
{
     this->point.erase(this->point.begin(), this->point.end());
     this->result.erase(this->result.begin(), this->result.end());
     update();

}

void UDrawWidget::mouseMoveEvent(QMouseEvent* pe)   // методы обработки события мыши при перемещении мыши
{
    if(catched_point == Point)
    {
        int dx = pe->x() - pr_mouseX;
        int dy = pe->y() - pr_mouseY;

        pr_mouseX = pe->position().x();
        pr_mouseY = pe->position().y();

        vector2d dd(dx, dy);
        for (int i = 0; i < int(point.size()); i++)
        {
            if (point[i] == k)
            {
                this->point[i] +=dd;
                k = point[i];
            }

        }

     this->repaint();
    }
}

void UDrawWidget::mouseReleaseEvent(QMouseEvent* pe) // методы обработки событий мыши при отжатии клавиши мыши
{
    catched_point = NoPoint;
}

