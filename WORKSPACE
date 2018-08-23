workspace(name = "com_github_jondo2010_flexcore")

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "776738aff8340592ea1b748859157c4df00853c8",
    remote = "https://github.com/jondo2010/rules_boost",
)

#local_repository(
#    #name = "boost",
#    name = "com_github_nelhage_rules_boost",
#    #build_file = "@com_github_nelhage_rules_boost//:BUILD.boost",
#    path = "/home/john/Source/rules_boost",
#)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

git_repository(
    name = "com_github_google_benchmark",
    commit =  "af441fc1143e33e539ceec4df67c2d95ac2bf5f8",
    remote = "https://github.com/google/benchmark",
)
