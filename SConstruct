import os
import glob

env = Environment(tools=['pdflatex','pdftex'])
env.Replace(ENV=os.environ)

for name in glob.glob('*.tex'):
  env.PDF(name[:-4]+'.pdf',name) 
