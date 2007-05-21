/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005, 2006 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLRESOURCE_H__
#define __ZLRESOURCE_H__

#include <string>

class ZLResource {

public:
	static const ZLResource &resource(const std::string &name);

protected:
	ZLResource();

public:
	virtual ~ZLResource();
	virtual bool hasValue() const = 0;
	virtual const std::string &value() const = 0;
	virtual const ZLResource &child(const std::string &name) const = 0;

private:
	// disable copying
	ZLResource(const ZLResource&);
	const ZLResource &operator = (const ZLResource&);
};

#endif /* __ZLRESOURCE_H__ */