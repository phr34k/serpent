import setuptools

setuptools.setup(
     name='serpent',  
     version='0.2',
     description="A msbuild conversion utility to the serpent specification",
     long_description="A msbuild conversion utility to the serpent specification",
     url="https://github.com/phr34k/serpent",
     packages=['serpent-msbuild'],
     include_package_data=True,     
     classifiers=[
         "Programming Language :: Python :: 2",
         "License :: OSI Approved :: MIT License",
         "Operating System :: Windows",
     ],
      entry_points={
          # 'console_scripts': [
          #     'srp-convert=serpent-msbuild.convert_project.main',
          # ]
      },     
 )