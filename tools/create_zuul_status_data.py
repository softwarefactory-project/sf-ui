#!/bin/env python
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

import argparse
import json
import logging
import os
from six.moves import urllib
import time

import pprint


class ZuulStatus:
    log = logging.getLogger("ZuulStatus")

    def __init__(self):
        p = argparse.ArgumentParser()
        p.add_argument("zuul_url")
        p.add_argument("--output")
        self.args = p.parse_args()
        self.cache = {}
        self.dirty_cache = False
        if (os.path.isfile(".cache")):
            self.cache = json.load(open(".cache"))

    def api(self, path):
        if path not in self.cache:
            url = "%s/api/%s" % (self.args.zuul_url, path)
            self.log.warning("Loading %s", url)
            try:
                time.sleep(.5)
                req = urllib.request.urlopen(url)
                self.cache[path] = json.loads(req.read().decode('utf-8'))
                self.dirty_cache = True
            except Exception:
                self.log.exception("Couldn't get %s", url)
                self.cache[path] = {}
        return self.cache[path]

    def resolveJob(self, tenant, job_name):
        job = self.api(os.path.join("tenant", tenant, "job", job_name))
        if not job:
            # Api bug when job has nested vars, assume base job
            job = self.api(os.path.join("tenant", tenant, "job", "base"))
        prev_job = {"name": None}
        nodeset = None
        while prev_job["name"] != job[0]["name"]:
            for j in job:
                if j.get("nodeset") is not None:
                    nodeset = j.get("nodeset")
                    break
            if nodeset is not None:
                break
            if job[0]["parent"] is None:
                break
            prev_job = job[0]
            job = self.api(
                os.path.join("tenant", tenant, "job", job[0]["parent"]))
            if not job:
                # Api bug when job has nested vars, assume base job
                job = self.api(os.path.join("tenant", tenant, "job", "base"))
        labels = []
        if nodeset:
            for node in nodeset["nodes"]:
                labels.append(node["label"])
        return {'labels': labels}

    def loadProject(self, tenant, project):
        project = self.api(os.path.join("tenant", tenant, "project", project))
        project_jobs = {}
        project_pipelines = set()
        for config in project['configs']:
            for pipeline in config['pipelines']:
                project_pipelines.add(pipeline["name"])
                for job in pipeline['jobs']:
                    if job[0]["name"] == "noop":
                        continue
                    project_jobs[job[0]['name']] = self.resolveJob(
                        tenant, job[0]['name'])
        project_jobs_list = []
        for job_name, data in sorted(project_jobs.items()):
            data["name"] = job_name
            project_jobs_list.append(data)
        return {'jobs': project_jobs_list,
                'pipelines': sorted(list(project_pipelines))}

    def loadTenant(self, tenant):
        projects = {}
        for project in self.api(os.path.join("tenant", tenant, "projects")):
            name = project['canonical_name']
            projects[name] = self.loadProject(tenant, name)
        projects_list = []
        for project_name, data in sorted(projects.items()):
            data["name"] = project_name
            projects_list.append(data)
        return {'projects': projects_list}

    def run(self):
        tenants = {}
        try:
            for tenant in self.api("tenants"):
                tenants[tenant['name']] = self.loadTenant(tenant['name'])
            tenants_list = []
            for tenant_name, data in sorted(tenants.items()):
                data["name"] = tenant_name
                tenants_list.append(data)
            if self.args.output.endswith(".js"):
                open(self.args.output, "w").write(
                    "const myJson = '%s';\nexport default myJson;" %
                    json.dumps({'tenants': tenants_list}))
            elif self.args.output.endswith(".json"):
                open(self.args.output, "w").write(json.dumps(
                    {'tenants': tenants_list}, sort_keys=True, indent=4))
            else:
                pprint.pprint(tenants_list)
        finally:
            if self.dirty_cache:
                json.dump(self.cache, open(".cache", "w"))


ZuulStatus().run()
