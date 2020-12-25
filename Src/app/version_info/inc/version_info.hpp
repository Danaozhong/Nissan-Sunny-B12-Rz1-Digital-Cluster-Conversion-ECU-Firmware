#ifndef _VERSION_INFO_HPP_
#define _VERSION_INFO_HPP_

namespace app
{
    const char* get_app_name();

    /** This will return the SW version as a C string. */
    const char* get_version_info();
    
    /** This will return the build type as a string (debug / release ) */
    const char* get_build_type();

    /** This will return the Git commit used to build this SW */
    const char* get_git_commit();
}
#endif /* _VERSION_INFO_HPP_ */
