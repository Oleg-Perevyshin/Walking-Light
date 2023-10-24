// Страница "СЕТЬ" (левая панель)

import {h, Component} from "preact";
import Message from "../components/Message";
import Button from "../components/Button";

export default class NetworkManager extends Component {
	render() {
		return (
			<div style="padding: 0 1em;">
				<h2 className={"tPan"}><Message key="m.l.t"/></h2>
				<p className={"indent justify"}><Message key="m.l.c"/></p>
				<p className={"indent justify"}><Message key="m.l.c1"/></p>
			</div>
		);
	}
}
