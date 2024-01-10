#include "pch.h"

#include "Model.h"

Model::Model(OpenGL* pOpenGL) :
m_pOpenGL(pOpenGL),
m_bSetup(false),
m_bLButtonDown(false),
m_rcItem{0},
m_ptLButtonDown{0} {}

Model::~Model() {}

void Model::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void Model::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void Model::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void Model::OnMouseMove(UINT nFlags, ::CPoint pt) {}

void Model::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}
