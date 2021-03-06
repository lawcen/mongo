// expression_internal.h

/**
 *    Copyright (C) 2013 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mongo/db/matcher/expression_internal.h"

namespace mongo {

    bool isAllDigits( const StringData& str ) {
        for ( unsigned i = 0; i < str.size(); i++ ) {
            if ( !isdigit( str[i] ) )
                return false;
        }
        return true;
    }

    string pathToString( const FieldRef& path, int32_t size ) {
        string res;
        if ( (size_t)size >= path.numParts() ) {
            return res;
        }

        res = path.getPart( size ).toString();
        for (size_t i = (size_t)size + 1; i < path.numParts(); i++ ) {
            res += ".";
            res += path.getPart( i ).toString();
        }
        return res;
    }

    BSONElement getFieldDottedOrArray( const BSONObj& doc,
                                       const FieldRef& path,
                                       int32_t* idxPath,
                                       bool* inArray )  {
        BSONElement res;

        BSONObj curr = doc;
        bool stop = false;
        size_t partNum = 0;
        while ( partNum < path.numParts() && !stop ) {

            res = curr.getField( path.getPart( partNum ) );

            switch ( res.type() ) {

            case EOO:
                stop = true;
                break;

            case Object:
                curr = res.Obj();
                ++partNum;
                break;

            case Array:
                if ( partNum+1 < path.numParts() && isAllDigits( path.getPart( partNum+1 ) ) ) {
                    *inArray = true;
                    curr = res.Obj();
                    ++partNum;
                }
                else {
                    stop = true;
                }
                break;

            default:
                if ( partNum+1 < path.numParts() ) {
                    res = BSONElement();
                }
                stop = true;

            }
        }

        *idxPath = partNum;
        return res;
    }


}  // namespace mongo
