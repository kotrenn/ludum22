#include "camera.h"

Camera::Camera(int w, int h, int space_w, int space_h,
               Surface *surface)
	:m_surface(NULL),
	 m_x(0), m_y(0),
	 m_w(w), m_h(h),
	 m_tx(0), m_ty(0),
	 m_space_w(space_w),
	 m_space_h(space_h),
	 m_target(NULL)
{
	if (surface == NULL)
		m_surface = new Surface(m_w, m_h);
	else
		m_surface = new Surface(surface);
}

Camera::~Camera()
{
	delete m_surface;
}

void Camera::move()
{
	if (m_target != NULL)
	{
		vector2d center = m_target->get_center();
		m_tx = (int)(center[0] - m_w / 2);
		m_ty = (int)(center[1] - m_h / 2);
	}

	int dx = m_tx - m_x;
	int dy = m_ty - m_y;
	m_x += (int)(0.05 * dx);
	m_y += (int)(0.05 * dy);

	if (m_x < 0)
		m_x = 0;
	if (m_y < 0)
		m_y = 0;
	if (m_x + m_w > m_space_w)
		m_x = m_space_w - m_w;
	if (m_y + m_h > m_space_h)
		m_y = m_space_h - m_h;
}

void Camera::clear()
{
	m_surface->fill(0);
}

void Camera::draw(Surface *dst, int x, int y) const
{
	dst->blit(m_surface, x, y);
}

const vector2d Camera::get_pos() const
{
	vector2d ret(m_x, m_y);
	return ret;
}

const vector2d Camera::get_size() const
{
	vector2d ret(m_w, m_h);
	return ret;
}

const Rect Camera::get_dims() const
{
	Rect ret(m_x, m_y, m_w, m_h);
	return ret;
}

void Camera::set_surface(Surface *surface)
{
	if (m_surface == surface)
		return;
	delete m_surface;
	m_surface = new Surface(surface);
}

void Camera::set_pos(const vector2d &pos)
{
	m_tx = (int)pos[0];
	m_ty = (int)pos[1];
}

void Camera::set_absolute(const vector2d &pos)
{
	m_x = m_tx = (int)pos[0];
	m_y = m_ty = (int)pos[1];
}
