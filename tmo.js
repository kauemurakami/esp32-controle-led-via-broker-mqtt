(function() {
	window.Main = {};
	Main.Page = (function() {
		var mosq = null;
		function Page() {
			var _this = this;
			mosq = new Mosquitto();

			$('#connect-button').click(function() {
				return _this.connect();
			});
			$('#disconnect-button').click(function() {
				return _this.disconnect();
			});
			$('#subscribe-button').click(function() {
				return _this.subscribe();
			});
			$('#unsubscribe-button').click(function() {
				return _this.unsubscribe();
			});
			
			//TESTE DE FUNÇÃO QUE RECEBE O ESTADO DE UM SENSOR

			
			
			// ----------------------------------
			//aqui vemos que com strings voce envia irform~ções para as validações na automatização
			$('#liga-output').click(function() {
				var payload = "L";  
				var TopicPublish = $('#pub-topic-text')[0].value;				
				mosq.publish(TopicPublish, payload, 0);
			});

			
			$('#desliga-output').click(function() {
				var payload = "D";  
				var TopicPublish = $('#pub-topic-text')[0].value;				
				mosq.publish(TopicPublish, payload, 0);
			});

			mosq.onconnect = function(rc){
				var p = document.createElement("p");
				var topic = $('#pub-subscribe-text')[0].value;
				p.innerHTML = "Conectado ao Broker!";
				$("#debug").append(p);
				mosq.subscribe(topic, 0);
				
			};
			mosq.ondisconnect = function(rc){
				var p = document.createElement("p");
				var url = "ws://iot.eclipse.org/ws";
				
				p.innerHTML = "A conexão com o broker foi perdida";
				$("#debug").append(p);				
				mosq.connect(url);
			};
			mosq.onmessage = function(topic, payload, qos){
				var p = document.createElement("p");
				//cria elemento para mostrar retorno dos sensores

				var acao = payload[0];
				if (acao == 'L')
					p.innerHTML = "<center><img src='ligado.png'><br>LIGADO</center>"
				else
					p.innerHTML = "<center><img src='desligado.png'><br>DESLIGADO</center>"
				$("#status_io").html(p);
			};
			//escreve o estado do output conforme informação recebida
				/*
				var acao2 = playload[0];
				var z = document.createElement("z");
				//VARIAVEL QUE RECEBE ESTADO CHAR ENVIADO DA PLACA COM MQTT.PUBLISHER				
				if(acao2 == 'S')
					z.innerHTML = "<center>Ligado</center>"				
				else (acao2 == 'F')
					z.innerHTML = "<center><p>Desligado</p></center>"
				$("#statusSensorTeste_io").html(z); */
			
			
		}
		Page.prototype.connect = function(){
			var url = "ws://iot.eclipse.org/ws";
			mosq.connect(url);
		};
		Page.prototype.disconnect = function(){
			mosq.disconnect();
		};
		Page.prototype.subscribe = function(){
			var topic = $('#sub-topic-text')[0].value;
			mosq.subscribe(topic, 0);
		};
		Page.prototype.unsubscribe = function(){
			var topic = $('#sub-topic-text')[0].value;
			mosq.unsubscribe(topic);
		};
		
		return Page;
	})();
	$(function(){
		return Main.controller = new Main.Page;
	});
}).call(this);

