from flask import Flask, request, render_template
from azure.storage.blob import AppendBlobService
import pandas
from datetime import datetime
import re

app = Flask(__name__)

@app.route("/")
def menu():
	append_blob_service = AppendBlobService(account_name='xxxxxxxxxxxxxxxxxxxxxxxxx', account_key='yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')
	append_blob_service.create_container('eem119')
	generator = []
	for x in append_blob_service.list_blobs('eem119'):
		generator.append(x.name)
	r = re.compile('\d\d\d\d-\d\d-\d\d-\d\d.*')
	generator = list(filter(r.match, generator))
	generator = set(x[:13] for x in generator)
	options = "<option value=\"" + request.base_url + "\">   </option>" + '\n' + "<option value=\"" + request.base_url + "current\">CURRENT</option>" + '\n' + "<option value=\"" + request.base_url + "timeplot\">TIMEPLOT</option>"
	for y in generator:
		print(y)
		options = options + '\n' + "<option value=\"" + request.base_url + "histogram/" + y + "\">" + y + "</option>"
	print(options)
	return render_template('menu.html', **locals())

@app.route("/timeplot")
def timeplot():
	timeplot_png1 = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/timeplot-heart.png"
	timeplot_png2 = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/timeplot-pedometer.png"
	timeplot_png3 = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/timeplot-accelerometer_xy.png"
	timeplot_png4 = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/timeplot-accelerometer_z.png"
	timeplot_png5 = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/timeplot-temperature.png"
	home = (request.base_url)[:-8]
	return render_template('timeplot.html', **locals())

@app.route("/histogram/<string:yyyymmddhh>/")
def histogram(yyyymmddhh):
	title = "HISTOGRAMS OF SENSOR VALUES AT " + yyyymmddhh[11:13] + " ON " + yyyymmddhh[5:7] + '-' + yyyymmddhh[8:10] + '-' + yyyymmddhh[0:4]
	heart_png = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/" + yyyymmddhh + "-heart.png"
	pedometer_png = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/" + yyyymmddhh + "-pedometer.png"
	accel_h_png = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/" + yyyymmddhh + "-acceleration_xy.png"
	accel_v_png = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/" + yyyymmddhh + "-acceleration_z.png"
	temperature_png = "https://xxxxxxxxxxxxxxxxxxxxxxxxx.blob.core.windows.net/eem119/" + yyyymmddhh + "-temperature.png"
	home = (request.base_url)[:-24]
	return render_template('histogram.html', **locals())

@app.route("/current")
def current():
	append_blob_service = AppendBlobService(account_name='xxxxxxxxxxxxxxxxxxxxxxxxx', account_key='yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')
	append_blob_service.create_container('eem119')
	today = datetime.today()
	dt = datetime.now()
	year = str(today.year)
	month = "{:02}".format(today.month)
	day = "{:02}".format(today.day)
	hour = "{:02}".format(dt.hour)
	#append_blob_service.get_blob_to_path('eem119', "2019-06-13-22.csv", 'current.csv')
	append_blob_service.get_blob_to_path('eem119', year + '-' + month + '-' + day + '-' + hour + ".csv", 'current.csv')
	content = pandas.read_csv('current.csv').to_html()
	home = (request.base_url)[:-8]
	return render_template('current.html', **locals())

@app.route("/error")
def error():
	append_blob_service = AppendBlobService(account_name='xxxxxxxxxxxxxxxxxxxxxxxxx', account_key='yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')
	append_blob_service.create_container('eem119')
	append_blob_service.get_blob_to_path('eem119', "error.log", 'error.log')
	content = pandas.read_csv('error.log').to_html()
	home = (request.base_url)[:-8]
	return render_template('current.html', **locals())
