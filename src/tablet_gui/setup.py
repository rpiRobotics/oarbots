from setuptools import find_packages, setup

package_name = 'tablet_gui'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer="Aidan O'Connor",
    maintainer_email='oconna4@rpi.edu',
    description='Launches the GUI for the OARBot tablet',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "tablet_gui = tablet_gui.main:main"
        ],
    },
)
