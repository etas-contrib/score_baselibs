..
    *******************************************************************************
    Copyright (c) 2025 Contributors to the Eclipse Foundation

    See the NOTICE file(s) distributed with this work for additional
    information regarding copyright ownership.

    This program and the accompanying materials are made available under the
    terms of the Apache License Version 2.0 which is available at
    https://www.apache.org/licenses/LICENSE-2.0

    SPDX-License-Identifier: Apache-2.0
    *******************************************************************************

Baselibs Documentation
============================

Welcome to the Baselibs documentation. This repository contains the foundational base libraries, providing essential components and utilities.

.. contents:: Table of Contents
   :depth: 2
   :local:

Purpose
-------
Provide a set of base libraries for C++ applications, ensuring reliability and performance across various platforms.

Summary
-------

**Library:** `Score Baselib`

**Type:** C++ Base Libraries

Module Documents
################

.. toctree::
   :maxdepth: 1
   :glob:

.. mod:: baselibs_cpp
   :id: mod__baselibs_cpp
   :includes: comp__log_cpp
   :status: valid
   :safety: ASIL_B
   :security: YES

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_module(need(), needs) }}

Components
==========

.. toctree::
   :titlesonly:
   :maxdepth: 2
   :glob:

   ./log_cpp/docs/index

