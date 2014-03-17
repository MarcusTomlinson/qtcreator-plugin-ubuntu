# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""qtcreator-plugin-ubuntu autopilot helpers."""

from autopilot.introspection.dbus import StateNotFoundError


# This will be the stand-in until we can get a proper CPO.
class FancyTabBar(object):
    def __init__(self, qtcreator_proxy):
        self._tabbar = qtcreator_proxy.select_single(
            "Core::Internal::FancyTabBar"
        )

    def get_plugin_tabs(self):
        """Returns a list of plugin tabs."""
        return self._tabbar.select_many("Core::Internal::FancyTab")

    def get_plugin_tab(self, tab_label):
        """Return a single tab with label *tab_label*

        :raises StateNotFoundError: If no tab found with the supplied label

        """
        return self._tabbar.select_single(
            "Core::Internal::FancyTab",
            text=tab_label
        )

    def activate_plugin_tab(self, tab_label):
        """Click on the tab with name *label*.

        Note: This code uses a slot within the QtCreator FancyTabBar plugin to
        accomplish this.

        :raises AssertionError: If the requested tab is not considered selected
        (by qtcreator).
        :raises StateNotFoundError: If a tab with the requested label isn't
        found.
        """
        try:
            self.get_plugin_tab(tab_label)
        except StateNotFoundError as e:
            e.args += (
                "Perhaps this plugin tab hasn't been loaded" % tab_label,
            )
            raise

        self._tabbar.slots.changeToTabWithLabel(tab_label)
        self._tabbar.selectedTabLabel.wait_for(tab_label)
