// Страница "СЦЕНАРИИ" (центр)

import {h, Component} from "preact";
import Scripts from "../components/Scripts";
import Message from "../components/Message";


export default class ScriptsPage extends Component {
	render() {
		return (
			<div>
				<h2 className="tPan"><Message key="sc.m.t"/></h2>
				<div>
					<Scripts/>
				</div>
			</div>
		);
	}
}
