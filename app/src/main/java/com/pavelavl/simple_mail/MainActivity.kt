package com.pavelavl.simple_mail

import android.annotation.SuppressLint
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.ListView
import android.widget.ScrollView
import android.widget.TextView
import android.widget.Toast
import android.util.Base64
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import java.util.Properties

class MainActivity : AppCompatActivity() {

	init {
		System.loadLibrary("native-lib")
	}

	external fun pop3Receive(): Array<String>
	external fun pop3Retrieve(index: Int): String
	external fun smtpSend(login: String, pass: String, from: String, to: String, subject: String, body: String)
	external fun setEnvVariable(key: String, value: String)

	private lateinit var listViewEmails: ListView
	private lateinit var txtMessage: TextView
	private lateinit var btnBack: Button
	private lateinit var txtMessageContainer: ScrollView
	private lateinit var SMTP_LOGIN: String
	private lateinit var SMTP_PASS: String

	@SuppressLint("MissingInflatedId")
	override fun onCreate(savedInstanceState: Bundle?) {
		super.onCreate(savedInstanceState)
		setContentView(R.layout.activity_main)
		loadEnvVariables()

		val edtFrom: EditText = findViewById(R.id.edtFrom)
		val edtTo: EditText = findViewById(R.id.edtTo)
		val edtSubject: EditText = findViewById(R.id.edtSubject)
		val edtBody: EditText = findViewById(R.id.edtBody)
		val btnGetEmails: Button = findViewById(R.id.btnGetEmails)
		txtMessage = findViewById(R.id.txtMessage)
		btnBack = findViewById(R.id.btnBack)
		listViewEmails = findViewById(R.id.listViewEmails)
		txtMessageContainer = findViewById(R.id.txtMessageContainer)

		btnBack.setOnClickListener {
			showEmailList()
		}
		btnGetEmails.setOnClickListener {
			loadEmails()
		}

		// Кнопка для отправки письма
		val btnSendEmail: Button = findViewById(R.id.btnSendEmail)
		btnSendEmail.setOnClickListener {
			val from = edtFrom.text.toString()
			val to = edtTo.text.toString()
			val subject = edtSubject.text.toString()
			val body = edtBody.text.toString()

			if (from.isNotEmpty() && to.isNotEmpty() && subject.isNotEmpty() && body.isNotEmpty()) {

				// Отправка письма
				smtpSend(SMTP_LOGIN, SMTP_PASS, from, to, subject, body)

				// Отображение сообщения об успешной отправке
				Toast.makeText(this, "Письмо отправлено!", Toast.LENGTH_SHORT).show()
			} else {
				Toast.makeText(this, "Заполните все поля!", Toast.LENGTH_SHORT).show()
			}
		}
	}

	private fun showEmailList() {
		listViewEmails.visibility = ListView.VISIBLE
		txtMessageContainer.visibility = ScrollView.GONE
		txtMessage.visibility = TextView.GONE
		btnBack.visibility = Button.GONE
	}

	private fun showMessage(message: String) {
		listViewEmails.visibility = ListView.GONE
		txtMessageContainer.visibility = ScrollView.VISIBLE
		txtMessage.visibility = TextView.VISIBLE
		btnBack.visibility = Button.VISIBLE
		txtMessage.text = message
	}

	private fun loadEmails() {
		val emails = pop3Receive()
		val adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, emails)
		listViewEmails.adapter = adapter

		listViewEmails.setOnItemClickListener { _, _, position, _ ->
			// Загрузка письма по индексу
			val message = pop3Retrieve(position) // Индекс с 1
			showMessage(message)
		}
	}

	fun encodeBase64(input: String): String {
		return Base64.encodeToString(input.toByteArray(Charsets.UTF_8), Base64.NO_WRAP)
	}

	private fun loadEnvVariables() {
		try {
			val properties = Properties()
			val inputStream = assets.open("local.properties") // Загрузка файла из assets
			properties.load(inputStream)

			// Передаем переменные в JNI
			setEnvVariable("POP3_SERVER", properties.getProperty("POP3_SERVER"))
			setEnvVariable("POP3_LOGIN", properties.getProperty("POP3_LOGIN"))
			setEnvVariable("POP3_PASS", properties.getProperty("POP3_PASS"))
			setEnvVariable("SMTP_SERVER", properties.getProperty("SMTP_SERVER"))
			SMTP_LOGIN = encodeBase64(properties.getProperty("SMTP_LOGIN"))
			SMTP_PASS = encodeBase64(properties.getProperty("SMTP_PASS"))

			Log.i("ENV", "Переменные среды успешно загружены")
		} catch (e: Exception) {
			Log.e("ENV", "Ошибка загрузки переменных среды", e)
		}
	}
}

