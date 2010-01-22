/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLResource.h>
#include <ZLOptionsDialog.h>

#include "LibraryNodes.h"
#include "BooksUtil.h"

#include "../library/Author.h"
#include "../fbreader/FBReader.h"

#include "AuthorInfoDialog.h"

class AuthorNode::EditInfoAction : public ZLRunnable {

public:
	EditInfoAction(shared_ptr<Author> author);
	void run();

private:
	shared_ptr<Author> myAuthor;
};

const std::string AuthorNode::TYPE_ID = "AuthorNode";

const std::string &AuthorNode::typeId() const {
	return TYPE_ID;
}

AuthorNode::AuthorNode(ZLBlockTreeView::RootNode *parent, size_t atPosition, shared_ptr<Author> author) : FBReaderNode(parent, atPosition), myAuthor(author) {
}

shared_ptr<Author> AuthorNode::author() const {
	return myAuthor;
}

void AuthorNode::paint(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("libraryView")["authorNode"];

	removeAllHyperlinks();

	drawCover(context, vOffset);
	drawTitle(context, vOffset,
		myAuthor.isNull() ? resource["unknownAuthor"].value() : myAuthor->name()
	);

	int left = 0;
	drawHyperlink(
		context, left, vOffset,
		resource[isOpen() ? "collapseTree" : "expandTree"].value(),
		expandTreeAction()
	);
	if (!myAuthor.isNull()) {
		if (myEditInfoAction.isNull()) {
			myEditInfoAction = new EditInfoAction(myAuthor);
		}
		drawHyperlink(
			context, left, vOffset,
			resource["edit"].value(),
			myEditInfoAction
		);
	}
}

AuthorNode::EditInfoAction::EditInfoAction(shared_ptr<Author> author) : myAuthor(author) {
}

void AuthorNode::EditInfoAction::run() {
	if (AuthorInfoDialog::run(myAuthor)) {
		// TODO: select current node (?) again
		FBReader::Instance().refreshWindow();
	}
}

shared_ptr<ZLImage> AuthorNode::extractCoverImage() const {
	return defaultCoverImage("booktree-author.png");
}