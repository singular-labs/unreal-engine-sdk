#!/usr/bin/env python3
import subprocess
import os
import shutil
import click


def run_script(script, stdin=None):
    """Returns (stdout, stderr), raises error on non-zero return code"""
    # Note: by using a list here (['bash', ...]) you avoid quoting issues, as the
    # arguments are passed in exactly this order (spaces, quotes, and newlines won't
    # cause problems):
    proc = subprocess.Popen(['bash', '-c', script],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            stdin=subprocess.PIPE)
    stdout, stderr = proc.communicate()
    if proc.returncode:
        raise ScriptException(proc.returncode, stdout, stderr, script)
    return stdout, stderr


class ScriptException(Exception):
    def __init__(self, returncode, stdout, stderr, script):
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr
        Exception.__init__('Error in script')


@click.command()
@click.option('-dry', is_flag=True, help='Dry run will build the SDK but will not sync to S3')
@click.option('-name', default=None, help='Change the name of the SDK. only works on dry runs')
def build_unity_sdk(dry, name):
    sdk_name = 'Singular'

    if name is not None:
        if not dry:
            print("Custom SDK name works only in dry mode")
            return

        sdk_name = name

    base_dir = '{0}SDK/'.format(sdk_name)
    plugins_dir = base_dir + 'Source/ThirdParty/'
    android_plugin_path = plugins_dir + 'Android/'
    ios_plugin_path = plugins_dir + 'iOS/'

    release_folder = 'release-folder/'
    if os.path.exists(release_folder):
        shutil.rmtree(release_folder)

    os.mkdir(release_folder)

    if name is not None:
        print("Copying native binaries...")
        binaries_dir = '../binaries/'
        for binary in os.listdir(binaries_dir):
            binary_path = binaries_dir + binary
            if '.aar' in binary:
                new_name = '{0}_sdk.aar'.format(sdk_name.lower())
                shutil.copyfile(binary_path, android_plugin_path + new_name)
            elif 'framework.zip' in binary:
                ios_binary_path = ios_plugin_path + binary
                shutil.copyfile(binary_path, ios_binary_path)

    output, err = run_script(
        'grep {0}SDK/Source/{0}SDK/Private/{0}SDKBPLibrary.cpp -e "#define UNREAL_ENGINE_SDK_VERSION"'.format(sdk_name))

    sdk_version = output.decode('utf-8').split('"')[1]

    package_name = '{0}SDK.{1}'.format(sdk_name, sdk_version)

    print('Building the SDK...')
    shutil.make_archive(package_name, 'zip', base_dir)
    zipped_package = package_name + '.zip'
    shutil.move(zipped_package, release_folder + zipped_package)

    os.chdir(release_folder)

    if not dry:
        print('Syncing the new version to S3')
        run_script('s3cmd put {0} s3://maven.singular.net/unreal-engine/{0}'.format(zipped_package))

    print('Done!')


if __name__ == '__main__':
    build_unity_sdk()
