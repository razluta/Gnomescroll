class Agent_controller_commands:
	
	def __init__(self):
		self.agent_controller = None
		self.agent_command_scheduler = None
		self.world_map = None
		#self.agents = None
		#self.objects = None
		self.globals = None


	# required = ['']
	# optional = ['']	
	def load_script(self, agent_id, script, **msg):
		self.agent_controller.load_script(agent_id, script) #load the script into agent controller
		self.agent_command_scheduler.executeNoop(agent_id) #starts running agent


### DEPRECATED BELOW LINE

	# required = ['']
	# optional = ['']	
	def move_agent_goal(self, agent_id, position, **msg):
		(type, x,y,z) = position
		self.agent_controller.create_move_goal(agent_id, x, y, z)
		self.agent_command_scheduler.executeNoop(agent_id)
		
	# required = ['']
	# optional = ['']
	def item_move_goal(self, agent_id, item_id, position, **msg):
		self.agent_controller. create_move_item_goal(agent_id, item_id, position)
		self.agent_command_scheduler.executeNoop(agent_id)
		
	# required = ['']
	# optional = ['']
	def load_job(self, agent_id, job_id, **msg):
		self.agent_controller.load_job(agent_id, job_id)
		self.agent_command_scheduler.executeNoop(agent_id)
