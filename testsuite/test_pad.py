#!/usr/bin/env python
# -*- Mode: Python -*-
# vi:si:et:sw=4:sts=4:ts=4
#
# GStreamer python bindings
# Copyright (C) 2002 David I. Lehn <dlehn@users.sourceforge.net>
#               2004 Johan Dahlin  <johan@gnome.org>

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

from common import gst, unittest

class PadTest(unittest.TestCase):
    def setUp(self):
        self.pipeline = gst.parse_launch('fakesrc name=source ! fakesink')
        src = self.pipeline.get_by_name('source')
        self.sink = src.get_pad('src')
        
    def testQuery(self):
        assert self.sink.query(gst.QUERY_TOTAL, gst.FORMAT_BYTES) == -1
        assert self.sink.query(gst.QUERY_POSITION, gst.FORMAT_BYTES) == 0
        assert self.sink.query(gst.QUERY_POSITION, gst.FORMAT_TIME) == 0

if __name__ == "__main__":
    unittest.main()
        
