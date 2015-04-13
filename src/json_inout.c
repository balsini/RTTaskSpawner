/*
This file is part of rt-app - https://launchpad.net/rt-app
Copyright (C) 2010  Giacomo Bagnoli <g.bagnoli@asidev.com>
Copyright (C) 2014  Juri Lelli <juri.lelli@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

//#include "rt-app_parse_config.h"

#include "json_inout.h"

#include <json/json.h>
#include <string.h>
#include <stdio.h>

#define PFX "[json] "
#define PFL "         "PFX
#define PIN PFX"    "
#define PIN2 PIN"    "
#define JSON_FILE_BUF_SIZE 4096
#define FILESIZE_JSON 1000 * 1000

/* redefine foreach as in <json/json_object.h> but to be ANSI
 * compatible */
#define foreach(obj, entry, key, val, idx)				\
  for ( ({ idx = 0; entry = json_object_get_object(obj)->head;});	\
  ({ if (entry) { key = (char*)entry->k;			\
  val = (struct json_object*)entry->v;	\
  };					\
  entry;						\
  }							\
  );							\
  ({ entry = entry->next; idx++; })			\
  )
/* this macro set a default if key not present, or give an error and exit
 * if key is present but does not have a default */
#define set_default_if_needed(key, value, have_def, def_value) do {	\
  if (!value) {							\
  if (have_def) {						\
  printf("key: %s <default> %d", key, def_value);\
  return def_value;				\
  } else {						\
  printf("Key %s not found", key);	\
  exit(2);				\
  }							\
  }								\
  } while(0)

/* same as before, but for string, for which we need to strdup in the
 * default value so it can be a literal */
#define set_default_if_needed_str(key, value, have_def, def_value) do {	\
  if (!value) {							\
  if (have_def) {						\
  if (!def_value) {				\
  printf("key: %s <default> NULL", key);\
  return NULL;				\
  }						\
  printf("key: %s <default> %s",		\
  key, def_value);			\
  return strdup(def_value);			\
  } else {						\
  printf("Key %s not found", key);	\
  exit(2);				\
  }							\
  }								\
  }while (0)

/* get an object obj and check if its type is <type>. If not, print a message
 * (this is what parent and key are used for) and exit
 */
static inline void
assure_type_is(struct json_object *obj,
               struct json_object *parent,
               const char *key,
               enum json_type type)
{
  if (!json_object_is_type(obj, type)) {
    printf("Invalid type for key %s", key);
    printf("%s", json_object_to_json_string(parent));
    exit(2);
  }
}

/* search a key (what) in object "where", and return a pointer to its
 * associated object. If nullable is false, exit if key is not found */
static inline struct json_object*
get_in_object(struct json_object *where,
              const char *what,
              int nullable)
{
  struct json_object *to;
  to = json_object_object_get(where, what);
  if (!nullable && is_error(to)) {
    printf("Error while parsing config:\n" PFL
           "%s", json_tokener_errors[-(unsigned long)to]);
    exit(2);
  }
  if (!nullable && strcmp(json_object_to_json_string(to), "null") == 0) {
    printf("Cannot find key %s", what);
    exit(2);
  }
  return to;
}

static inline int
get_int_value_from(struct json_object *where,
                   const char *key,
                   int have_def,
                   int def_value)
{
  struct json_object *value;
  int i_value;
  value = get_in_object(where, key, have_def);
  set_default_if_needed(key, value, have_def, def_value);
  assure_type_is(value, where, key, json_type_int);
  i_value = json_object_get_int(value);
  json_object_put(value);
  printf("key: %s, value: %d, type <int>\n", key, i_value);
  return i_value;
}

static inline int
get_double_value_from(struct json_object *where,
                   const char *key,
                   double have_def,
                   double def_value)
{
  struct json_object *value;
  double i_value;
  value = get_in_object(where, key, have_def);
  assure_type_is(value, where, key, json_type_double);
  i_value = json_object_get_double(value);
  json_object_put(value);
  printf("key: %s, value: %f, type <double>\n", key, i_value);
  return i_value;
}

static inline int
get_bool_value_from(struct json_object *where,
                    const char *key,
                    int have_def,
                    int def_value)
{
  struct json_object *value;
  int b_value;
  value = get_in_object(where, key, have_def);
  set_default_if_needed(key, value, have_def, def_value);
  assure_type_is(value, where, key, json_type_boolean);
  b_value = json_object_get_boolean(value);
  json_object_put(value);
  printf("key: %s, value: %d, type <bool>", key, b_value);
  return b_value;
}

static inline char*
get_string_value_from(struct json_object *where,
                      const char *key,
                      int have_def,
                      const char *def_value)
{
  struct json_object *value;
  char *s_value;
  value = get_in_object(where, key, have_def);
  set_default_if_needed_str(key, value, have_def, def_value);
  if (json_object_is_type(value, json_type_null)) {
    printf("key: %s, value: NULL, type <string>", key);
    return NULL;
  }
  assure_type_is(value, where, key, json_type_string);
  s_value = strdup(json_object_get_string(value));
  json_object_put(value);
  printf("key: %s, value: %s, type <string>", key, s_value);
  return s_value;
}

/*
static void
parse_resources(struct json_object *resources, rtapp_options_t *opts)
{
    int i;
    int res = json_object_get_int(resources);
    printf("Creating %d resources", res);
    opts->resources = malloc(sizeof(rtapp_resource_t) * res);
    for (i = 0; i < res; i++) {
        pthread_mutexattr_init(&opts->resources[i].mtx_attr);
        if (opts->pi_enabled) {
            pthread_mutexattr_setprotocol(
                &opts->resources[i].mtx_attr,
                PTHREAD_PRIO_INHERIT);
        }
        pthread_mutex_init(&opts->resources[i].mtx,
                   &opts->resources[i].mtx_attr);
        opts->resources[i].index = i;
    }
    opts->nresources = res;
}
*/

/*
static void
serialize_acl(rtapp_resource_access_list_t **acl,
          int idx,
          struct json_object *task_resources,
          rtapp_resource_t *resources)
{
    int i, next_idx, found;
    struct json_object *access, *res, *next_res;
    rtapp_resource_access_list_t *tmp;
    char s_idx[5];

    // as keys are string in the json, we need a string for searching
    // the resource
    snprintf(s_idx, 5, "%d", idx);

    if (!(*acl)) {
        *acl = malloc( sizeof(rtapp_resource_access_list_t));
        (*acl)->res = &resources[idx];
//		(*acl)->index = idx;
        (*acl)->next = NULL;
        (*acl)->prev = NULL;
        tmp = *acl;
    } else {
        found = 0;
        tmp = *acl;
        while (tmp->next != NULL) {
            if (tmp->res->index == idx)
                found = 1;
            tmp = tmp->next;
        }
        if (found == 0) {
            // add the resource to the acl only if it is not already
            // present in the list
            tmp->next = malloc ( sizeof (rtapp_resource_access_list_t));
            // tmp->next->index = idx;
            tmp->next->next = NULL;
            tmp->next->prev = tmp;
            tmp->next->res = &resources[idx];
        }
    }

    res = get_in_object(task_resources, s_idx, TRUE);
    if (!res)
        return;
    assure_type_is(res, task_resources, s_idx, json_type_object);

    access = get_in_object(res, "access", TRUE);
    if (!access)
        return;
    assure_type_is(access, res, "access", json_type_array);

    for (i=0; i<json_object_array_length(access); i++)
    {
        next_res = json_object_array_get_idx(access, i);
        if (!json_object_is_type(next_res, json_type_int)){
            printf("Invalid resource index");
            exit(2);
        }
        next_idx = json_object_get_int(next_res);
        // recurse on the rest of resources
        serialize_acl(&(*acl), next_idx, task_resources, resources);
    }
}
*/

/*
static void
parse_thread_resources(const rtapp_options_t *opts, struct json_object *locks,
               struct json_object *task_resources, thread_data_t *data)
{
    int i,j, cur_res_idx, usage_usec;
    struct json_object *res;
    int res_dur;
    char res_name[4];

    rtapp_resource_access_list_t *tmp, *head, *last;
    char debug_msg[512], tmpmsg[512];

    data->blockages = malloc(sizeof(rtapp_tasks_resource_list_t) *
                 json_object_array_length(locks));
    data->nblockages = json_object_array_length(locks);
    for (i = 0; i< json_object_array_length(locks); i++)
    {
        res = json_object_array_get_idx(locks, i);
        if (!json_object_is_type(res, json_type_int)){
            printf("Invalid resource index");
            exit(2);
        }
        cur_res_idx = json_object_get_int(res);

        data->blockages[i].usage = usec_to_timespec(0);
        data->blockages[i].acl = NULL;
        serialize_acl(&data->blockages[i].acl, cur_res_idx,
                task_resources, opts->resources);

        // since the "current" resource is returned as the first
        // element in the list, we move it to the back
        tmp = data->blockages[i].acl;
        head = data->blockages[i].acl;
        do {
            last = tmp;
            tmp = tmp->next;
        } while (tmp != NULL);
        // move first element to list end
        if (last != head) {
            data->blockages[i].acl = head->next;
            data->blockages[i].acl->prev = NULL;
            last->next = head;
            head->next = NULL;
            head->prev = last;
        }

        tmp = data->blockages[i].acl;
        debug_msg[0] = '\0';
        do  {
            snprintf(tmpmsg, 512, "%s %d", debug_msg, tmp->res->index);
            strncpy(debug_msg, tmpmsg, 512);
            last = tmp;
            tmp = tmp->next;
        } while (tmp != NULL);

        printf("key: acl %s", debug_msg);

        snprintf(res_name, 4, "%d", cur_res_idx);
        res = get_in_object(task_resources, res_name, TRUE);
        if (!res) {
            usage_usec = 0;
            data->blockages[i].usage = usec_to_timespec(0);
        } else {
            assure_type_is(res, task_resources, res_name,
                    json_type_object);
            usage_usec = get_int_value_from(res, "duration", TRUE, 0);
            data->blockages[i].usage = usec_to_timespec(usage_usec);
        }
        printf("res %d, usage: %d acl: %s", cur_res_idx,
              usage_usec, debug_msg);
    }
}
*/


static void
parse_thread_data(char *name, struct json_object *obj, int idx,
                  periodic_task_attr *p/*, const rtapp_options_t *opts*/)
{
    long period, deadline, s_period, s_deadline, s_runtime, jobs, ss_every, ss, c0, c1;

    printf("Parsing thread %s [%d]\n", name, idx);

    // period
    period = get_double_value_from(obj, "period", FALSE, 0);
    if (period <= 0) {
        printf("Cannot set negative period");
        exit(2);
    }
    p->period = period;

    // deadline
    deadline = get_double_value_from(obj, "deadline", FALSE, 0);
    if (deadline <= 0) {
        printf("Cannot set negative deadline");
        exit(2);
    }
    p->deadline = deadline;

    // s_period
    s_period = get_double_value_from(obj, "s_period", FALSE, 0);
    if (s_period <= 0) {
        printf("Cannot set negative s_period");
        exit(2);
    }
    p->s_period = s_period;

    // s_deadline
    s_deadline = get_double_value_from(obj, "s_deadline", FALSE, 0);
    if (s_deadline <= 0) {
        printf("Cannot set negative s_deadline");
        exit(2);
    }
    p->s_deadline = s_deadline;

    // s_runtime
    s_runtime = get_double_value_from(obj, "s_runtime", FALSE, 0);
    if (s_runtime <= 0) {
        printf("Cannot set negative s_runtime");
        exit(2);
    }
    p->s_runtime = s_runtime;

    // jobs
    jobs = get_double_value_from(obj, "jobs", FALSE, 0);
    if (jobs <= 0) {
        printf("Cannot set negative jobs");
        exit(2);
    }
    p->jobs = jobs;

    // ss_every
    ss_every = get_double_value_from(obj, "ss_every", FALSE, 0);
    if (ss_every < 0) {
        printf("Cannot set negative ss_every");
        exit(2);
    }
    p->ss_every = ss_every;

    // ss
    ss = get_double_value_from(obj, "ss", FALSE, 0);
    if (ss <= 0) {
        printf("Cannot set negative ss");
        exit(2);
    }
    p->ss = ss;

    // c0
    c0 = get_double_value_from(obj, "c0", FALSE, 0);
    if (c0 <= 0) {
        printf("Cannot set negative c0");
        exit(2);
    }
    p->c0 = c0;

    // c1
    c1 = get_double_value_from(obj, "c1", FALSE, 0);
    if (c1 <= 0) {
        printf("Cannot set negative c1");
        exit(2);
    }
    p->c1 = c1;


    /*
    // exec time
    exec = get_int_value_from(obj, "exec", FALSE, 0);
    if (exec > period) {
        printf("Exec must be greather than period");
        exit(2);
    }
    if (exec < 0) {
        printf("Cannot set negative exec time");
        exit(2);
    }
    data->min_et = usec_to_timespec(exec);
    data->max_et = usec_to_timespec(exec);

    // policy
    policy_to_string(opts->policy, def_policy);
    policy = get_string_value_from(obj, "policy", TRUE, def_policy);
    if (policy) {
        if (string_to_policy(policy, &data->sched_policy) != 0) {
            printf("Invalid policy %s", policy);
            exit(2);
        }
    }
    policy_to_string(data->sched_policy, data->sched_policy_descr);

    // priority
    data->sched_prio = get_int_value_from(obj, "priority", TRUE,
                          DEFAULT_THREAD_PRIORITY);

    // deadline
    dline = get_int_value_from(obj, "deadline", TRUE, period);
    if (dline < exec) {
        printf("Deadline cannot be less than exec time");
        exit(2);
    }
    if (dline > period) {
        printf("Deadline cannot be greater than period");
        exit(2);
    }
    data->deadline = usec_to_timespec(dline);

    // cpu set
    cpuset_obj = get_in_object(obj, "cpus", TRUE);
    if (cpuset_obj) {
        assure_type_is(cpuset_obj, obj, "cpus", json_type_array);
        data->cpuset_str = json_object_to_json_string(cpuset_obj);
        printf("key: cpus %s", data->cpuset_str);
        data->cpuset = malloc(sizeof(cpu_set_t));
        cpuset = json_object_get_array(cpuset_obj);
        CPU_ZERO(data->cpuset);
        for (i=0; i < json_object_array_length(cpuset_obj); i++) {
            cpu = json_object_array_get_idx(cpuset_obj, i);
            cpu_idx = json_object_get_int(cpu);
            CPU_SET(cpu_idx, data->cpuset);
        }
    } else {
        data->cpuset_str = strdup("-");
        data->cpuset = NULL;
        printf("key: cpus %s", data->cpuset_str);
    }

    // resources
    resources = get_in_object(obj, "resources", TRUE);
    locks = get_in_object(obj, "lock_order", TRUE);
    if (locks) {
        assure_type_is(locks, obj, "lock_order", json_type_array);
        printf("key: lock_order %s", json_object_to_json_string(locks));
        if (resources) {
            assure_type_is(resources, obj, "resources",
                    json_type_object);
            printf("key: resources %s",
                  json_object_to_json_string(resources));
        }
        parse_thread_resources(opts, locks, resources, data);
    }
    */
}

static void
parse_tasks(struct json_object *tasks, periodic_task_attr *p[], unsigned int *size/*, rtapp_options_t *opts*/)
{
  // used in the foreach macro
  periodic_task_attr *pta;
      struct lh_entry *entry; char *key; struct json_object *val; int idx;
  unsigned int nthreads = 0;

  printf("Parsing threads section\n");

  foreach(tasks, entry, key, val, idx) {
    nthreads++;
  }
  printf("Found %d threads\n", nthreads);

  pta = (periodic_task_attr *)malloc(sizeof(periodic_task_attr) * nthreads);

  foreach (tasks, entry, key, val, idx)
    parse_thread_data(key, val, idx, &pta[idx]/*, opts*/);

  *size = nthreads;
  *p = pta;
}

/*
static void
parse_global(struct json_object *global, rtapp_options_t *opts)
{
    char *policy;
    printf("Parsing global section");
    opts->spacing = get_int_value_from(global, "spacing", TRUE, 0);
    opts->duration = get_int_value_from(global, "duration", TRUE, -1);
    opts->gnuplot = get_bool_value_from(global, "gnuplot", TRUE, 0);
    policy = get_string_value_from(global, "default_policy",
                       TRUE, "SCHED_OTHER");
    if (string_to_policy(policy, &opts->policy) != 0) {
        printf("Invalid policy %s", policy);
        exit(2);
    }
    opts->logdir = get_string_value_from(global, "logdir", TRUE, NULL);
    opts->lock_pages = get_bool_value_from(global, "lock_pages", TRUE, 1);
    opts->logbasename = get_string_value_from(global, "log_basename",
                          TRUE, "rt-app");
    opts->ftrace = get_bool_value_from(global, "ftrace", TRUE, 0);
    opts->pi_enabled = get_bool_value_from(global, "pi_enabled", TRUE, 0);
#ifdef AQUOSA
    opts->fragment = get_int_value_from(global, "fragment", TRUE, 1);
#endif

}
*/

static void
get_opts_from_json_object(struct json_object *root, periodic_task_attr *p[], unsigned int *size/*, rtapp_options_t *opts*/)
{
  struct json_object *tasks;//, *resources, *global;

  if (is_error(root)) {
    printf("Error while parsing input JSON: %s\n",
           json_tokener_errors[-(unsigned long)root]);
    exit(2);
  }
  printf("Successfully parsed input JSON\n");
  printf("\nroot     : %s\n\n", json_object_to_json_string(root));

  //global = get_in_object(root, "global", FALSE);
  //printf("global   : %s\n", json_object_to_json_string(global));

  tasks = get_in_object(root, "tasks", FALSE);
  printf("tasks    : %s\n", json_object_to_json_string(tasks));
  /*
    resources = get_in_object(root, "resources", FALSE);
    printf("resources: %s\n", json_object_to_json_string(resources));

    */
  //parse_global(global, opts);
  //parse_resources(resources, opts);
  parse_tasks(tasks, p/*, opts*/, size);

}

void
parse_config_stdin(periodic_task_attr *p[], unsigned int *size)
{
  /* read from stdin until EOF, write to temp file and parse
     * as a "normal" config file */
  size_t in_length;
  //char buf[JSON_FILE_BUF_SIZE];
  char *buf;
  struct json_object *js;
  printf("Reading JSON config from stdin...\n");

  buf = (char *)malloc(sizeof(char) * FILESIZE_JSON);

  //in_length = fread(buf, sizeof(char), JSON_FILE_BUF_SIZE, stdin);
  in_length = fread(buf, sizeof(char), FILESIZE_JSON, stdin);
  buf[in_length] = '\0';
  js = json_tokener_parse(buf);
  get_opts_from_json_object(js, p, size);
  return;
}

/*
void
parse_config(const char *filename)
{
  //int done;
  char *fn = strdup(filename);
  struct json_object *js;
  printf("Reading JSON config from %s", fn);
  js = json_object_from_file(fn);
  get_opts_from_json_object(js);
  return;
}
*/
