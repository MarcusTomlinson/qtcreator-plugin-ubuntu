# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""qtcreator-plugin-ubuntu autopilot tests."""

from autopilot.testcase import AutopilotTestCase
from qtcreator_plugin_ubuntu.helpers import FancyTabBar


class QtCreatorSideMenuTests(AutopilotTestCase):

    def test_ubuntu_tab_option_exists(self):
        app_proxy = self.launch_test_application("qtcreator")
        tabbar = FancyTabBar(app_proxy)

        tab = tabbar.get_plugin_tab('Ubuntu')

        self.assertTrue(tab.enabled)

    def test_IRC_tab_option_exists(self):
        app_proxy = self.launch_test_application("qtcreator")
        tabbar = FancyTabBar(app_proxy)

        tab = tabbar.get_plugin_tab('IRC')

        self.assertTrue(tab.enabled)

    def test_can_enable_IRC_mode(self):
        app_proxy = self.launch_test_application("qtcreator")
        tabbar = FancyTabBar(app_proxy)

        tabbar.activate_plugin_tab("IRC")
